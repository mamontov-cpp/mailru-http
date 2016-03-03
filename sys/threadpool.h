/*! \file threadool.h

    Describes a simple threadpool
*/
#pragma once
#include "thread.h"
#include <vector>
#include <uv.h>

namespace sys
{

class Worker;
    
class ThreadPool
{
public:
    /*! Construct new thread pool (note, that function will get link to a running flag)
        \param[in] count amount of pools
     */
    ThreadPool(int count);
    /*! Destroys thread pool
     */
    ~ThreadPool();
    /*! Enqueues a socket in thread pool
        \param[in] socket a socket
     */
    void enqueue(uv_tcp_t* socket);
    /*! Runs all threads
        \returns count of threads started
     */
    int run();
    /*! Joins all threads, forcing them too stop
     */
    void wait();
protected:
    /*! A list of threads
     */
    std::vector<sys::Thread* > m_threads;
    /*! A list of workers
     */
    std::vector<sys::Worker* > m_workers;
};


}