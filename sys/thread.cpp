#include "thread.h"

#ifdef WIN32

#include <process.h>

sys::ThreadId sys::current_thread_id()
{
    return 	GetCurrentThread();
}

#endif

#ifdef LINUX

 #include <linux/unistd.h>
 #include <sys/types.h>
 
sys::ThreadId sys::current_thread_id()
{
    return  syscall(SYS_gettid);
}

#endif


sys::Thread::Thread(sys::ThreadFunction f, void* arg)
: m_function(f), m_arg(arg),
#ifdef WIN32
  m_handle(INVALID_HANDLE_VALUE)
#else
  m_handle(0)
#endif
{

}

sys::Thread::Thread(const sys::Thread & o)
: m_function(o.m_function), m_arg(o.m_arg), m_handle(o.m_handle)
{
#ifdef WIN32
    // Duplicate handle, for handle closing handle
    DuplicateHandle(GetCurrentProcess(), 
                    o.m_handle, 
                    GetCurrentProcess(),
                    &m_handle,
                    0,
                    TRUE,
                    DUPLICATE_SAME_ACCESS
                   );
#endif
}

sys::Thread::~Thread()
{
#ifdef WIN32
    if (m_handle != INVALID_HANDLE_VALUE)
    {
        CloseHandle(m_handle);
    }
#endif
}


#ifdef WIN32

struct Args
{
sys::ThreadFunction F;
void* A;
};

static unsigned int WINAPI thread_implementation_function(LPVOID args)
{
    Args * f = reinterpret_cast<
        Args *
    >(args);
    int code = reinterpret_cast<int>((f->F)(f->A));
    _endthreadex(code);
    delete f;
    return static_cast<unsigned int>(code);
}

#endif


bool sys::Thread::run()
{   
#ifdef WIN32    
    Args* a = new Args();
    a->F = m_function;
    a->A = m_arg;
    m_handle = reinterpret_cast<HANDLE>(_beginthreadex(NULL, 0, thread_implementation_function, a, 0, NULL));
    return m_handle != NULL;
#else
    pthread_attr_t attrs;
    pthread_attr_init(&attrs);
    int result = pthread_create(&m_handle,&attrs,m_function, m_arg);
    bool running = (result == 0);
    return running;
#endif
}


void sys::Thread::wait() const
{
#ifdef WIN32
        WaitForSingleObject(m_handle, INFINITE);
#else
        void * result = NULL;
        pthread_join(m_handle,&result);
#endif
}
