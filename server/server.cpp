#include "server.h"
#include "../config.h"
#include "../sys/log.h"
#include "../cmd/parse.h"
#include "../regex/dfa.h"

extern cmd::Options opts;

/* A dfa responsible for parsing HTTP 0.9 GET-header
 */
static regex::DFA HTTP_09_GET_string;
/* A dfa responsible for parsing HTTP 1.0 GET-header
 */
static regex::DFA HTTP_10_GET_string;


void server::Server::initParser()
{
    HTTP_09_GET_string.stringShouldContain("GET ").startCaptureGroup();
    HTTP_09_GET_string.zeroOrInfinityNot('\r', ' ').endCaptureGroup();
    HTTP_09_GET_string.stringMustEndWidth('\n');

    HTTP_10_GET_string.stringShouldContain("GET ").startCaptureGroup();
    HTTP_10_GET_string.zeroOrInfinityNot(' ').endCaptureGroup();
    HTTP_10_GET_string.zeroOrInfinityNot('\r');
    HTTP_10_GET_string.stringMustEndWidth('\n');
}

server::RequestParsingState* server::Server::parseRequest(server::ClientState* state)
{
    server::RequestParsingState* result = new server::RequestParsingState();
    result->Complete = false;
    result->Valid = false;
    if (state->Request.size() != 0)
    {
        // Those regexes allow too much, but we can ignore it
        regex::MatchResult match_HTTP_09_GET = HTTP_09_GET_string.match(state->Request);
        regex::MatchResult match_HTTP_10_GET = HTTP_10_GET_string.match(state->Request);
        if ((match_HTTP_09_GET.Type != regex::MRT_ERROR) || (match_HTTP_10_GET.Type != regex::MRT_ERROR) )
        {
            result->Valid = true;
#ifdef NO_DAEMONIZE
            sys::Log::write(
                "Matched HTTP 0.9 header: %s\nMatched HTTP 1.0 header: %s\n", 
                match_HTTP_09_GET.Result[0].c_str(), 
                match_HTTP_10_GET.Result[0].c_str() 
            );
#endif
            if (match_HTTP_10_GET.Type ==  regex::MRT_FULL)
            {
                result->Complete = true;
                result->URI = match_HTTP_10_GET.Result[1];
            }

            if (match_HTTP_09_GET.Type ==  regex::MRT_FULL)
            {
                result->Complete = true;
                result->URI = match_HTTP_09_GET.Result[1];
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