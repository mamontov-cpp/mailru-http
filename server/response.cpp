#include "response.h"
#include <string>
#include <sstream>
#include "../config.h"
#include "../sys/log.h"

server::Response::Response() : m_code(0)
{
    
}

char* server::Response::body() const
{
    return const_cast<char*>(&(m_body[0]));
}

int server::Response::length() const
{
    return m_body.size();
}

int server::Response::code() const
{
    return m_code;
}

void server::Response::setBody(const std::string& body)
{
    m_body = body;
}

void server::Response::setCode(int code)
{
    m_code = code;
}

server::Response* server::Response::makeResponse(int code, const std::string& reason, const std::string& body)
{
    std::ostringstream stream;
    stream << "HTTP/1.0 " << code << " " << reason << "\r\n";
    stream << "Connection: close\r\n";
    stream << "Content-type: text/html\r\n";
    stream << "Content-length: " << body.length() << "\r\n\r\n";
    stream << body;

    server::Response* response = new server::Response();
    response->setCode(code);
    response->setBody(stream.str());
#ifdef NO_DAEMONIZE
    // Commented to not pollute log
    // sys::Log::write("[INFO] Made a response:\n%s\n", stream.str().c_str());
#endif
    return response;
}

server::Response* server::respond400()
{
    return server::Response::makeResponse(400, "Bad Request", "Your request was so bad, that we could not even parse it");
}

server::Response* server::respond404()
{
    return server::Response::makeResponse(404, "Not Found", "Nothing is found. This is sad");
}

server::Response* server::respond200(const std::string& body)
{
    return server::Response::makeResponse(200, "OK", body);    
}