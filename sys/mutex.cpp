#include "mutex.h"
#include "log.h"

sys::Mutex::Mutex()
{
    if (uv_mutex_init(&m_mtx) != 0)
    {
        sys::Log::write("[ERROR] Could not create mutex, possible resource exhaustion");
    }
}

sys::Mutex::~Mutex()
{
    uv_mutex_destroy(&m_mtx);
}

// ReSharper disable once CppMemberFunctionMayBeConst
void sys::Mutex::lock()
{
    uv_mutex_lock(&m_mtx);
}

// ReSharper disable once CppMemberFunctionMayBeConst
void sys::Mutex::unlock()
{
    uv_mutex_unlock(&m_mtx);
}
