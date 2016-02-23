#include "cmd/parse.h"
#include "cmd/daemonize.h"
#include "server/on_connect.h"
#include "sys/log.h"
#include "uv.h"

#ifndef WIN32
    #include <signal.h>
#endif

uv_loop_t* uv_loop;
uv_tcp_t server_socket;
cmd::Options opts;

#define MAX_WRITE_HANDLES 1000
#define CHECK(status, msg)                                           \
if (status != 0) {                                                   \
    sys::Log::write("[FATAL] %s: %s\n", msg, uv_err_name(status));   \
    return 2;                                                        \
}

int main(int argc, char** argv)
{
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
        result = uv_ip4_addr(opts.IP.c_str(), static_cast<int>(opts.Port), &address);
        CHECK(result, "uv_ip4_addr");
        result = uv_tcp_bind(&server_socket, reinterpret_cast<const struct sockaddr*>(&address), 0);
        CHECK(result, "uv_tcp_bind");
        result = uv_listen(reinterpret_cast<uv_stream_t*>(&server_socket), MAX_WRITE_HANDLES, server::on_connect);
        CHECK(result, "uv_listen");
        sys::Log::write("[INFO] Starting listening %s on %s:%d\n", opts.Directory.c_str(), opts.IP.c_str(), opts.Port);
        uv_run(uv_loop,UV_RUN_DEFAULT);
    }
    else
    {
        printf("[FATAL] Failed to daemonize!");
    }
    return 0;
}