#include "cmd/parse.h"
#include <cstdio>

int main(int argc, char** argv)
{
    cmd::Options opts;
    cmd::parse(argc, argv, opts);
    printf("Going to serve %s on %s:%d\n", opts.Directory.c_str(), opts.IP.c_str(), opts.Port);
    return 0;
}