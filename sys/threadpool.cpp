#include "threadpool.h"
#include "worker.h"

sys::ThreadPool::ThreadPool(int count)
{
    for(int i = 0; i < count; i++)
    {
        sys::Worker* wrk = new sys::Worker();
        if (wrk->init())
        {
            m_workers.push_back(wrk);
            m_threads.push_back(new sys::Thread(sys::Worker::startInThread, wrk));
        } 
        else
        {
            delete wrk;
        }
    }
}

sys::ThreadPool::~ThreadPool()
{
    for(size_t i = 0; i < m_threads.size(); i++)
    {
        delete m_threads[i];
    }
     for(size_t i = 0; i < m_workers.size(); i++)
    {
        delete m_workers[i];
    }
}

void sys::ThreadPool::enqueue(uv_tcp_t* socket)
{
    int pos = -1;
    size_t minqueue = 0;
    for(size_t i = 0; i < m_workers.size(); i++)
    {
        size_t tmp = m_workers[i]->queueSize();
        if (tmp == 0)
        {
            m_workers[i]->enqueue(socket);
            return;
        }

        if ((pos == -1) || (tmp < minqueue))
        {
            minqueue = tmp;
            pos = i;
        }
    }

    if (pos != -1)
    {
        m_workers[pos]->enqueue(socket);
    }
}
int sys::ThreadPool::run()
{
    int result = 0;
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
    for(size_t i = 0; i < m_workers.size(); i++)
    {
        m_workers[i]->kill();
    }
    for(size_t i = 0; i < m_threads.size(); i++)
    {
        m_threads[i]->wait();
    }
}