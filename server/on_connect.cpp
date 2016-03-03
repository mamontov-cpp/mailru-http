#include "on_connect.h"
#include "clientstate.h"
#include "server.h"
#include "../sys/log.h"
#include "../sys/mutex.h"
#include <deque>
#include "../config.h"
// ReSharper disable once CppUnusedIncludeDirective
#include <vector>
// ReSharper disable once CppUnusedIncludeDirective
#include <cstdlib>
// ReSharper disable once CppUnusedIncludeDirective
#include <cstring>

extern uv_loop_t* uv_loop;

static std::deque<uv_tcp_t*> clients;
static sys::Mutex clients_lock;

void server::on_connect(uv_stream_t* server_handle, int status)
{
    if (status == 0)
    {
        uv_tcp_t* client = new uv_tcp_t();
        uv_tcp_init(uv_loop, client);
        int result = uv_accept(server_handle, reinterpret_cast<uv_stream_t*>(client));
        if (result == 0)
        {
            //on_accept(client);
            clients_lock.lock();
            clients.push_back(client);
            clients_lock.unlock();
        }
        else
        {
#if UV_VERSION_MAJOR < 1
            sys::Log::write("[WARNING] Error on accept: %s\n", uv_err_name(uv_last_error(uv_loop)));
#else
            sys::Log::write("[WARNING] Error on accept: %s\n", uv_err_name(result));
#endif
            uv_close(reinterpret_cast<uv_handle_t *>(client), NULL);
            delete client;
        }
    }
    else
    {
#if UV_VERSION_MAJOR < 1
        sys::Log::write("[WARNING] Error on connect: %s\n", uv_err_name(uv_last_error(uv_loop)));
#else
        sys::Log::write("[WARNING] Error on connect: %s\n", uv_err_name(status));
#endif
    }
}


void* server::on_accept(void* socket)
{
    uv_tcp_t* client = reinterpret_cast<uv_tcp_t*>(socket);
    sys::Log::write("[INFO] Client %p connected\n", client);
    
    client->data = new ClientState();

    uv_read_start(reinterpret_cast<uv_stream_t*>(client), server::on_memory_request, server::on_read);
    return NULL;
}

void server::on_close(uv_handle_t* handle)
{
    sys::Log::write("[INFO] Client %p disconnected\n", handle); 
    delete reinterpret_cast<uv_tcp_t*>(handle);
}

#if UV_VERSION_MAJOR < 1 
uv_buf_t server::on_memory_request(uv_handle_t* handle, size_t sz)
{
    sys::Log::write("[INFO] Client %p requested buffer of %d bytes\n", handle, sz); 
    char* memory = new char[sz];
    return uv_buf_init(memory, sz);
}
#else
void server::on_memory_request(uv_handle_t* handle, size_t sz, uv_buf_t* buf)
{
    sys::Log::write("[INFO] Client %p requested buffer of %d bytes\n", handle, sz); 
    char* memory = new char[sz];
    *buf = uv_buf_init(memory, sz);
}
#endif

void server::on_read(
    uv_stream_t* stream, 
    ssize_t nread,
#if UV_VERSION_MAJOR < 1    
    uv_buf_t buf
#else
    uv_buf_t const*  buf
#endif
    )
{
    server::ClientState* state = reinterpret_cast<server::ClientState*>(stream->data);
    /* if read bytes counter -1 there is an error or EOF */

#ifdef NO_DAEMONIZE
        sys::Log::write("[INFO] Read callback called on %p with %d\n", stream, nread);
#endif

    if (nread < 0) {
        if (nread != UV_EOF) {
#if UV_VERSION_MAJOR < 1
            sys::Log::write("[WARNING] Error on reading from buffer on %p: %s\n", stream, uv_err_name(uv_last_error(uv_loop)));
#else
            sys::Log::write("[WARNING] Error on reading from buffer on %p: %s\n", stream, uv_err_name(nread));
#endif
        }
        
        server::check_if_can_respond(stream, state, true);

        if (stream->data)
        {
            delete state;
        }
        uv_close(reinterpret_cast<uv_handle_t *>(stream), server::on_close);
    }
    else
    {
        // Copy buffer to inner vector
        if (nread > 0)
        {
            std::string buffer_length_string_for_log;
            buffer_length_string_for_log.resize(nread);
#if UV_VERSION_MAJOR < 1
            memcpy(&(buffer_length_string_for_log[0]), buf.base, nread);
            state->append(buf.base, nread);            
#else
            memcpy(&(buffer_length_string_for_log[0]), buf->base, nread);
            state->append(buf->base, nread);
#endif
            sys::Log::write("[INFO] Received request on %p:\n%s\n", stream, buffer_length_string_for_log.c_str());
            server::check_if_can_respond(stream, state, false);
        }
    }


    /* free the remaining memory */
#if UV_VERSION_MAJOR < 1
    if (buf.base && nread > 0)
    {
        free(buf.base);
    }
#else
    if (buf->base && nread > 0)
    {
        free(buf->base);
    }
#endif
}


void server::check_if_can_respond(
    uv_stream_t* stream, 
    server::ClientState* state,
    bool require
)
{
    if (state->Responded == false)
    {
        server::RequestParsingState* rps = server::Server::parseRequest(state);
        server::Response* response = NULL; 
        if (require)
        {    
            response = server::Server::stateToResponse(rps);
        }
        else
        {
            if (rps->Complete)
            {
                response = server::Server::stateToResponse(rps);
            }
        }
        if (response)
        {
            state->Responded = true;

            uv_buf_t out_buf;
            out_buf.base = response->body();
            out_buf.len = response->length();
            sys::Log::write(
                "[INFO] Piping to %p response %d in request for %s(%s)\n", 
                stream,  
                response->code(),
                rps->BaseURI.c_str(),
                rps->URI.c_str()
            );

            uv_write_t* req = new uv_write_t();
            int result = uv_write(req, stream, &out_buf, 1, NULL);
            if (result) 
            {
                #if UV_VERSION_MAJOR < 1
                    sys::Log::write("[WARNING] Error on writing to buffer on %p: %s\n", stream, uv_err_name(uv_last_error(uv_loop)));
                #else
                    sys::Log::write("[WARNING] Error on writing to buffer on %p: %s\n", stream, uv_err_name(result));
                #endif
            }
            delete response;
        }
        delete rps;
    }
}

uv_tcp_t* server::next_connection()
{       
    uv_tcp_t* result = NULL;
    // Too lazy to use actual semaphore here
    clients_lock.lock();
    if (clients.size())
    {
        result = clients.front();
        clients.pop_front();
    }
    clients_lock.unlock();
    return result;    
}

void server::worker_function(void* arg)
{
    bool* running = static_cast<bool*>(arg);
    while((*running))
    {
        
        uv_tcp_t* client = server::next_connection();
        if (client)
        {
            server::on_accept(client);
        }
    }
}
