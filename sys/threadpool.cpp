#include "threadpool.h"

sys::ThreadPool::ThreadPool(sys::ThreadFunction f, int count)
: m_fun(f), m_running(false) 
{
    for(int i = 0; i < count; i++)
    {
        m_threads.push_back(new sys::Thread(f, &m_running));
    }
}

sys::ThreadPool::~ThreadPool()
{
    for(size_t i = 0; i < m_threads.size(); i++)
    {
        delete m_threads[i];
    }
}

void sys::ThreadPool::run()
{
    m_running = true;
    for(size_t i = 0; i < m_threads.size(); i++)
    {
        m_threads[i]->run();
    }
}

void sys::ThreadPool::wait()
{
    m_running = false;
    for(size_t i = 0; i < m_threads.size(); i++)
    {
        m_threads[i]->wait();
    }
}