/*! \file      thread.h

    Contains crossplatform thread implementation,  
    which uses pthread on Linux and WinAPI on Windows 
*/
#pragma once

#ifdef WIN32
    #ifndef NOMINMAX
    #define NOMINMAX 
    #endif
    #include  <windows.h>
#else
#ifndef _GNU_SOURCE
    #define _GNU_SOURCE
#endif
    #include  <pthread.h>
    #include  <sys/types.h>
    #include  <sys/syscall.h>
    #include  <linux/unistd.h>
    #include <unistd.h>
#endif

namespace sys
{

typedef void*(*ThreadFunction)(void*);

#ifdef WIN32
    typedef HANDLE ThreadId;
#endif

#ifdef LINUX
    typedef pid_t ThreadId;
#endif

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
    void wait() const;
protected:
    /*! Defines executed code
     */
    sys::ThreadFunction m_function;
    /*! Argument for function
     */
    void* m_arg;
#ifdef WIN32
    /*! A handle, working with thread
     */
    HANDLE m_handle;
#else
    /*! A handle, working with thread
     */
    pthread_t m_handle;
#endif
private:
    /*! DO NOT USE! Not implemented
        \param[in] o other object
        \return self-reference
     */
    sys::Thread & operator=(const sys::Thread& o);
};



}
