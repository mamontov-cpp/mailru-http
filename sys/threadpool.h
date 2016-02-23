/*! \file threadool.h

    Describes a simple threadpool
*/
#pragma once
#include "thread.h"
#include <vector>

namespace sys
{
    
class ThreadPool
{
public:
    /*! Construct new thread pool (note, that function will get link to a running flag)
        \param[in] f function
        \param[in] count amount of pools
     */
    ThreadPool(sys::ThreadFunction f, int count);
    /*! Destroys thread pool
     */
    ~ThreadPool();
    /*! Runs all threads
     */
    void run();
    /*! Joins all threads, forcing them too stop
     */
    void wait();
protected:
    /*! A function
     */
    sys::ThreadFunction m_fun;
    /*! Whether thread pool is running
     */
    bool m_running;
    /*! A list of threads
     */
    std::vector<sys::Thread* > m_threads;
};


}