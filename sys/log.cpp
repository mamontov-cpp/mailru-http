#include "log.h"
#include "../config.h"

static sys::Log m_log;

sys::Log::Log() : m_file(NULL)
{
    
}

sys::Log::~Log()
{
    if (m_file)
    {
        fclose(m_file);
    }
}

bool sys::Log::open()
{
    // This ridiculous name is picked to avoid conflicts with other testing names
#ifdef WIN32
    m_log.m_file = fopen("__________log.txt", "wt");
#else
    m_log.m_file = fopen("/home/box/__________log.txt", "wt");
#endif
    if (m_log.m_file)
    {
        setvbuf(m_log.m_file, NULL, _IONBF, 0);
    }
    return m_log.m_file != NULL;
}

void sys::Log::write(const char* format, ...)
{
    if (m_log.m_file)
    {
        m_log.m_mtx.lock();
        va_list args;

        va_start(args, format);
#ifdef NO_DAEMONIZE
        vprintf(format, args);
#endif
        vfprintf(m_log.m_file, format, args);
        va_end(args);
        m_log.m_mtx.unlock();
    }
}