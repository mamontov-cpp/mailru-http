#include "on_connect.h"
#include "../sys/log.h"
#include "../sys/thread.h"
#include "../config.h"
#include <vector>
#include <cstdlib>

extern uv_loop_t* uv_loop;

void server::on_connect(uv_stream_t* server_handle, int status)
{
    if (status == 0)
    {
        uv_tcp_t* client = new uv_tcp_t();
        uv_tcp_init(uv_loop, client);
        int result = uv_accept(server_handle, reinterpret_cast<uv_stream_t*>(client));
        if (result == 0)
        {
            // Too lazy to put queue here.
            sys::Thread thread(server::on_accept, client);
            thread.run();
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
    
    client->data = new std::vector<char>();

    uv_read_start(reinterpret_cast<uv_stream_t*>(client), server::on_memory_request, server::on_read);
    return NULL;
}

void server::on_close(uv_handle_t* handle)
{
    sys::Log::write("[INFO] Client %p disconnected\n", handle); 
    delete reinterpret_cast<uv_tcp_t*>(handle);
}

#if UV_VERSION_MAJOR < 1 
uv_buf_t* server::on_memory_request(uv_handle_s* handle, size_t sz)
{
    sys::Log::write("[INFO] Client %p requested buffer of %d bytes\n", handle, sz); 
    char* memory = new char[sz];
    uv_buf_t* buf = new uv_buf_t();
    *buf = uv_buf_init(memory, sz);
    return buf
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
    int nread, 
#if UV_VERSION_MAJOR < 1    
    uv_buf_t buf
#else
    uv_buf_t const*  buf
#endif
    )
{
    uv_write_t req;
    /* if read bytes counter -1 there is an error or EOF */
    if (nread < 0) {
        if (nread != UV_EOF) {
#if UV_VERSION_MAJOR < 1
            sys::Log::write("[WARNING] Error on reading from buffer on %p: %s\n", stream, uv_err_name(uv_last_error(uv_loop)));
#else
            sys::Log::write("[WARNING] Error on reading from buffer on %p: %s\n", stream, uv_err_name(nread));
#endif
        }
        

        /* write sync the incoming buffer to the socket */
        uv_buf_t out_buf;
        char tmp[1] = {0};
        if (stream->data)
        {
            std::vector<char>* l = reinterpret_cast<std::vector<char> *>(stream->data);
            out_buf.base = &((*l)[0]);
            out_buf.len = l->size();
#ifdef NO_DAEMONIZE
            l->push_back(0);
            sys::Log::write("[INFO] Piping to %p buffer %d: %s\n", stream, l->size(), &((*l)[0]));
#endif
        }
        else
        {
            out_buf.len = 1;
            out_buf.base = tmp;
        }

        int result = uv_write(&req, stream, &out_buf, 1, NULL);
        if (result) {
#if UV_VERSION_MAJOR < 1
            sys::Log::write("[WARNING] Error on writing to buffer on %p: %s\n", stream, uv_err_name(uv_last_error(uv_loop)));
#else
            sys::Log::write("[WARNING] Error on writing to buffer on %p: %s\n", stream, uv_err_name(result));
#endif
        }


        if (stream->data)
        {
            delete reinterpret_cast<std::vector<char> *>(stream->data);
        }
        uv_close(reinterpret_cast<uv_handle_t *>(stream), server::on_close);
    }
    else
    {
        // Copy buffer to inner vector
        if (nread > 0)
        {
            std::vector<char>* vector = reinterpret_cast<std::vector<char> *>(stream->data);
#if UV_VERSION_MAJOR < 1
            for(int i = 0; i < nread; i++)
            {
                vector->push_back(buf.base[i]);
            }
#else
            for(int i = 0; i < nread; i++)
            {
                vector->push_back(buf->base[i]);
            }
#endif
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