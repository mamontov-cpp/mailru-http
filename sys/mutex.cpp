#include "mutex.h"

#ifdef WIN32
    #include  <windows.h> 
#endif

sys::Mutex::Mutex()
{
#ifdef WIN32
    m_m = new CRITICAL_SECTION();
    InitializeCriticalSection(m_m);
#else
    pthread_mutex_init(&m_m,NULL);
#endif
}

sys::Mutex::~Mutex()
{
#ifdef WIN32
    DeleteCriticalSection(m_m);
    delete m_m;
#else
    pthread_mutex_destroy(&m_m);
#endif
}

void sys::Mutex::lock() const
{
#ifdef WIN32
    EnterCriticalSection(m_m);
#else
    pthread_mutex_lock(&m_m);
#endif
}

void sys::Mutex::unlock() const
{
#ifdef WIN32
    LeaveCriticalSection(m_m);
#else
    pthread_mutex_unlock(&m_m);
#endif
}
