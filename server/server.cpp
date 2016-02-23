#include "server.h"
#include "../config.h"
#include "../sys/log.h"
#include "../slre/slre.h"
#include "../cmd/parse.h"

extern cmd::Options opts;

server::RequestParsingState* server::Server::parseRequest(server::ClientState* state)
{
    server::RequestParsingState* result = new server::RequestParsingState();
    result->Complete = false;
    result->Valid = false;
    if (state->Request.size() != 0)
    {
        // Those regexes allow too much, but we can ignore it
        int matched_simple = slre_match("^G?E?T?\\s[^\\r]*\\r?\\n?", &(state->Request[0]), state->Request.size(), NULL, 0, 0);
        int matched_one = slre_match("^G?E?T?\\s[^\\s]*\\s?[^\\r]*\\r?\\n?", &(state->Request[0]), state->Request.size(), NULL, 0, 0);
        if (matched_one > 0 || matched_simple > 0)
        {
            result->Valid = true;
#ifdef NO_DAEMONIZE
            sys::Log::write("Matched 0.9: %d, 1.0: %d", matched_simple, matched_one);
#endif
            slre_cap caps09;
            slre_cap caps10;
            matched_simple = slre_match("^GET\\s([^\\r]+)\\r\n", &(state->Request[0]), state->Request.size(), &caps09, 1, 0);
            matched_one = slre_match("^GET\\s([^\\s]+)\\s[^\\r]*\\r\n", &(state->Request[0]), state->Request.size(), &caps10, 1, 0);
            if (matched_one > 0)
            {
                result->Complete = true;
                result->URI = std::string(caps10.ptr, caps10.ptr + caps10.len);

                // Due to SLRE being very buggy, it places spaces into URI
                size_t smark = result->URI.find(" ");
                if (smark != std::string::npos)
                {
                    result->URI = result->URI.substr(0, smark);
                }
            }

            if (matched_simple > 0)
            {
                result->Complete = true;
                result->URI = std::string(caps09.ptr, caps09.ptr + caps09.len);

                // Due to SLRE being very buggy, it places spaces into URI
                size_t smark = result->URI.find(" ");
                if (smark != std::string::npos)
                {
                    result->URI = result->URI.substr(0, smark);
                }
            }
            if (result->Complete)
            {
               size_t qmark = result->URI.find('?');
               if (qmark != std::string::npos)
               {
                   result->BaseURI = result->URI.substr(0, qmark);
               }
               else
               {
                   result->BaseURI = result->URI;
               }
            }
        }
        else
        {
            result->Complete = true;
            result->Valid = false;
        }
    }
    else
    {
        result->Valid = true;
    }
    return result;
}

server::Response* server::Server::stateToResponse(server::RequestParsingState* state)
{
    server::Response* response;
    if (state->Complete == false)
    {
        response = server::respond400();
    }
    else
    {
        std::string folder = opts.Directory;
        if (folder.length() == 0)
        {
            folder = ".";
        }
        if (folder[folder.length() - 1] == '/' || folder[folder.length() - 1] == '\\')
        {
            folder = folder.substr(0, folder.length() - 1);
        }
        if (state->BaseURI == "/")
        {
            state->BaseURI = "/index.html";
        }
        std::string pt = folder + state->BaseURI;
        FILE* file = fopen(pt.c_str(), "rb");
        if (file)
        {
            fseek(file, 0, SEEK_END);
            long int filesize = ftell(file);
            std::string content;
            content.resize(filesize);
            fseek(file, 0, SEEK_SET);
            fread(&(content[0]), 1, filesize, file);
            fclose(file);
            response =  server::respond200(content);
        }
        else
        {
            response = server::respond404();            
        }
    }
    return response;
}