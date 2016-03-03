/*! \file worker.h
    
    A worker class for thread pool
 */
#include <uv.h>
#include <deque>
#include "mutex.h"

namespace sys
{
    
class Worker
{
public:
    /*! Makes new worker
     */
    Worker();
    /*! Tries to initialize worker
     */
    bool init();
    /*! A worker
     */
    ~Worker();
    /*! Fetches a socket from queue
     */
    void perform();
    /*! Makes worker run a loop
        \param[in] worker a reference for worker
     */
    static void startInThread(void* worker);
    /*!  Used to send signal for killing loop in worker
     */
    void kill();
    /*! Sends a queue size
        \returns a size of queue
     */
    size_t queueSize() const;
    /*! Dequeues worker's queue
        \return next socket or null
     */
    uv_tcp_t* dequeue() const;
    /*! Pushes socket in queue
        \param[in] socket a socket data
     */
    void enqueue(uv_tcp_t* socket);
    /*! Callback, which is called, when worker receives socket
        \param[in] handle handle
        \param[in] status a status for callback
     */
#if UV_VERSION_MAJOR < 1
    static void onSocketReceived(uv_async_t* handle, int status);
#else
    static void onSocketReceived(uv_async_t* handle);
#endif
    /*! Callback, which is called, when worker receives kill signal
        \param[in] handle handle
        \param[in] status a status for callback
     */
#if UV_VERSION_MAJOR < 1
    static void onKillReceived(uv_async_t* handle, int status);
#else
    static void onKillReceived(uv_async_t* handle);
#endif
protected:
    /*! Queue for sockets to be handled
     */
    std::deque<uv_tcp_t*> m_queue;
    /*! A lock for queue
     */
    sys::Mutex* m_mtx;
    /*! A loop for worker thread
     */
    uv_loop_t* m_loop;
    /*! Signal, which should force worker loop to die
     */
    uv_async_t m_kill_signal;
    /*! Received signal async
     */
    uv_async_t m_receive_signal;
};


}
