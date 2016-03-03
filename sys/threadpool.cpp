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

int sys::ThreadPool::run()
{
    int result = 0;
    m_running = true;
    for(size_t i = 0; i < m_threads.size(); i++)
    {
        if (m_threads[i]->run())
        {
            ++result;
        }
    }
    return result;
}

void sys::ThreadPool::wait()
{
    m_running = false;
    for(size_t i = 0; i < m_threads.size(); i++)
    {
        m_threads[i]->wait();
    }
}