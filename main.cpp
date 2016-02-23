#include "cmd/parse.h"
#include "cmd/daemonize.h"
#include "server/on_connect.h"
#include "sys/log.h"
#include "sys/threadpool.h"
#include <uv.h>


// Либа полагается на libuv и slre для разбора

#ifndef WIN32
    #include <signal.h>
#endif

uv_loop_t* uv_loop;
uv_tcp_t server_socket;
cmd::Options opts;

#define MAX_WRITE_HANDLES 1000
#if UV_VERSION_MAJOR < 1
#define CHECK(status, msg)                                                    \
if (status != 0) {                                                            \
    sys::Log::write("[FATAL] %s: %s\n", msg, uv_err_name(uv_last_error(uv_loop)));   \
    return 2;                                                                 \
}
#else
#define CHECK(status, msg)                                           \
if (status != 0) {                                                   \
    sys::Log::write("[FATAL] %s: %s\n", msg, uv_err_name(status));   \
    return 2;                                                        \
}
#endif

int main(int argc, char** argv)
{
    sys::ThreadPool pool(server::worker_function, 10);
    cmd::parse(argc, argv, opts);
    printf("[INFO] Going to serve %s on %s:%d\n", opts.Directory.c_str(), opts.IP.c_str(), opts.Port);
    if (sys::Log::open() == false)
    {
        printf("[FATAL] Cannot open log file");
        return 1;
    }

    if (cmd::daemonize())
    {
#ifndef WIN32
        signal(SIGPIPE, SIG_IGN);
#endif
        uv_loop = uv_default_loop();
        int result = uv_tcp_init(uv_loop, &server_socket);
        CHECK(result, "uv_tcp_init");
        result = uv_tcp_keepalive(&server_socket,1,60);
        CHECK(result, "uv_tcp_keepalive");
        struct sockaddr_in address;
#if UV_VERSION_MAJOR < 1
        address = uv_ip4_addr(opts.IP.c_str(), static_cast<int>(opts.Port));
        result = 0;
#else
        result = uv_ip4_addr(opts.IP.c_str(), static_cast<int>(opts.Port), &address);
#endif
        CHECK(result, "uv_ip4_addr");
#if UV_VERSION_MAJOR < 1
        result = uv_tcp_bind(&server_socket, address);
#else
        result = uv_tcp_bind(&server_socket, reinterpret_cast<const struct sockaddr*>(&address), 0);
#endif
        CHECK(result, "uv_tcp_bind");
        result = uv_listen(reinterpret_cast<uv_stream_t*>(&server_socket), MAX_WRITE_HANDLES, server::on_connect);
        CHECK(result, "uv_listen");
        sys::Log::write("[INFO] Starting listening %s on %s:%d\n", opts.Directory.c_str(), opts.IP.c_str(), opts.Port);
        pool.run();
        uv_run(uv_loop,UV_RUN_DEFAULT);
        pool.wait();
    }
    else
    {
        printf("[FATAL] Failed to daemonize!");
    }
    return 0;
}