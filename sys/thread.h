/*! \file      thread.h

    Contains crossplatform thread implementation,  
    which uses pthread on Linux and WinAPI on Windows 
*/
#pragma once
#include "uv.h"

namespace sys
{

typedef void(*ThreadFunction)(void*);
typedef uv_thread_t ThreadId;

/*! Returns current thread id
    \return thread id
 */
sys::ThreadId current_thread_id();

/*! A platform-dependent implementation of thread, which uses WinAPI on Win32 and
    pthreads on linux
 */
class Thread
{
public:
    /*! Constructs new implementation for thread
        \param[in] f function, which should be executed when thread is run
        \param[in] arg argument
     */
    Thread(sys::ThreadFunction f, void* arg);
    /*! Copies a thread implementation
        \param[in] o other implementation
     */
    Thread(const sys::Thread & o);
    /*! Closes handle, if need to
     */
    ~Thread();
    /*! Runs a thread
        \return true if succeeded, otherwise false
     */
    bool run();
    /*! Blocks execution and waits until thread is stopped
     */
    void wait();
protected:
    /*! Defines executed code
     */
    sys::ThreadFunction m_function;
    /*! Argument for function
     */
    void* m_arg;
    /*! A handle for thread
     */
    uv_thread_t m_handle;
private:
    /*! DO NOT USE! Not implemented
        \param[in] o other object
        \return self-reference
     */
    sys::Thread & operator=(const sys::Thread& o);
};



}
