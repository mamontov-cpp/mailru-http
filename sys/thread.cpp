#include "thread.h"

sys::ThreadId sys::current_thread_id()
{
    return 	uv_thread_self();
}

sys::Thread::Thread(sys::ThreadFunction f, void* arg)
: m_function(f), m_arg(arg), m_handle()
{

}

sys::Thread::Thread(const sys::Thread & o)
: m_function(o.m_function), m_arg(o.m_arg), m_handle(o.m_handle)
{

}

sys::Thread::~Thread()
{

}

bool sys::Thread::run()
{   
    return uv_thread_create(&m_handle, m_function, m_arg) == 0;
}


void sys::Thread::wait()
{
    uv_thread_join(&m_handle);
}
