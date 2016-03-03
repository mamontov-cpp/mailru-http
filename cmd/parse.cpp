#include "parse.h"
#include <stdio.h>

#ifdef WIN32
    #include "../getopt/getopt.h"
#else
    #include <unistd.h>
#endif


void cmd::parse(int argc, char** argv, cmd::Options& opts)
{
    int c;
    while ((c = getopt (argc, argv, "h:p:d:t:")) != -1)
    switch (c)
    {
      case 'h':
        opts.IP = optarg;
        break;
      case 'p':
        if (sscanf(optarg, "%d", &opts.Port) < 1)
        {
            opts.Port = 80;
        }
        break;
      case 't':
        if (sscanf(optarg, "%u", &opts.ThreadPoolSize) < 1)
        {
            opts.ThreadPoolSize = 80;
        }
        break;
      case 'd':
        opts.Directory = optarg;
        break;      
    }
}