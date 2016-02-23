#include "on_connect.h"
#include "../sys/log.h"
#include "../sys/thread.h"
#include "../config.h"
#include <vector>

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
            sys::Thread thread(server::on_accept, client);
            thread.run();
        }
        else
        {
            sys::Log::write("[WARNING] Error on accept: %s\n", uv_err_name(result));
            uv_close(reinterpret_cast<uv_handle_t *>(client), NULL);
            delete client;
        }
    }
    else
    {
        sys::Log::write("[WARNING] Error on connect: %s\n", uv_err_name(status));
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

void server::on_memory_request(uv_handle_t* handle, size_t sz, uv_buf_t* buf)
{
    sys::Log::write("[INFO] Client %p requested buffer of %d bytes\n", handle, sz); 
    char* memory = new char[sz];
    *buf = uv_buf_init(memory, sz);
}

void server::on_read(uv_stream_t* stream, int nread, uv_buf_t const*  buf)
{
    uv_write_t req;
    /* if read bytes counter -1 there is an error or EOF */
    if (nread < 0) {
        if (nread != UV_EOF) {
            sys::Log::write("[WARNING] Error on reading from buffer on %p: %s\n", stream, uv_err_name(nread));
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
            sys::Log::write("[WARNING] Error on writing to buffer on %p: %s\n", stream, uv_err_name(result));
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
            for(int i = 0; i < nread; i++)
            {
                vector->push_back(buf->base[i]);
            }
        }
    }


    /* free the remaining memory */
    if (buf->base && nread > 0)
    {
        free(buf->base);
    }
}