/*! \file on_connect.h
    
    Describes an actions, which will be performed on connection
 */
#pragma once
#include <uv.h>

namespace server
{

/*! Describes a thing for on connecting
    \param[in] server_handle a server handles
    \param[in] status an access status
 */
void on_connect(uv_stream_t* server_handle, int status);
/*! An inner thread, which launches after accepting connection
    \param[in] socket an accepted socket
    \return NULL
 */
void* on_accept(void* socket);
/*! A callback, which is called on closing handle
    \param[in] handle a handle
 */
void on_close(uv_handle_t* handle);

#if UV_VERSION_MAJOR < 1
/*! A callback for requesting memory
    \param[in] handle a handle
    \param[in] a requested size
    \param[in] buf a buffer
 */
uv_buf_t* on_memory_request(uv_handle_t* handle, size_t sz);
/*! A callback, which is called on readable state
    \param[in] stream a stream
    \param[in] nread a readable state
    \param[in] buf buffer
 */
void on_read(uv_stream_t * stream, int nread, uv_buf_t buf);
#else
/*! A callback for requesting memory
    \param[in] handle a handle
    \param[in] a requested size
    \param[in] buf a buffer
 */
void on_memory_request(uv_handle_t* handle, size_t sz, uv_buf_t* buf);
/*! A callback, which is called on readable state
    \param[in] stream a stream
    \param[in] nread a readable state
    \param[in] buf buffer
 */
void on_read(uv_stream_t * stream, int nread, uv_buf_t const*  buf);
#endif

}