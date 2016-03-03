#include "worker.h"
#include "../server/on_connect.h"

sys::Worker::Worker()
: m_mtx(NULL), m_loop(NULL)
{
    
}

bool sys::Worker::init()
{
    m_mtx = new sys::Mutex();
    m_loop = uv_loop_new();
    m_loop->data = this;
    bool result = true;
    result = result && (uv_async_init(m_loop, &m_kill_signal, &sys::Worker::onKillReceived)  == 0);
    result = result && (uv_async_init(m_loop, &m_receive_signal, &sys::Worker::onSocketReceived)  == 0);
    return result;
}

sys::Worker::~Worker()
{
    delete m_mtx;
    uv_loop_delete(m_loop);    
}

// ReSharper disable once CppMemberFunctionMayBeConst
void sys::Worker::perform()
{
    uv_tcp_t* client = this->dequeue();
    client->loop = m_loop;
    if (client)
    {
        server::on_accept(client);
    }
}

void sys::Worker::startInThread(void* worker)
{
    sys::Worker* w = static_cast<sys::Worker*>(worker);    
    uv_run(w->m_loop,UV_RUN_DEFAULT);
}

void sys::Worker::kill()
{
    uv_async_send(&m_kill_signal);
}

size_t sys::Worker::queueSize() const
{
    m_mtx->lock();
    size_t result = m_queue.size();
    m_mtx->unlock();
    return result;
}

uv_tcp_t* sys::Worker::dequeue() const
{
    uv_tcp_t* result = NULL;
    m_mtx->lock();
    if (m_queue.size())
    {
        result = m_queue.front();
        const_cast<Worker*>(this)->m_queue.pop_front();
    }
    m_mtx->unlock();
    return result;  
}

void sys::Worker::enqueue(uv_tcp_t* socket)
{
    m_mtx->lock();
    m_queue.push_back(socket);
    m_mtx->unlock();
    uv_async_send(&m_receive_signal);
}


void sys::Worker::onSocketReceived(uv_async_t* handle)
{
    reinterpret_cast<sys::Worker*>(handle->loop->data)->perform();
}

void sys::Worker::onKillReceived(uv_async_t* handle)
{
    uv_stop(handle->loop);
}
