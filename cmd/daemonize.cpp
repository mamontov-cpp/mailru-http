#include "daemonize.h"
#include "../config.h"
#include <cstdio>

#ifdef WIN32
    #ifdef _MSC_VER
        #ifndef NO_DAEMONIZE
            #pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
        #endif
    #endif
#else
    #include <unistd.h>
#endif

bool cmd::daemonize()
{
#ifndef NO_DAEMONIZE
    #ifdef WIN32
        return true;
    #else
        return daemon(1, 0) != -1;
    #endif
#else
    return true;
#endif
}