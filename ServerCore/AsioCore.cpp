#include "pch.h"
#include "AsioCore.h"

AsioCore::AsioCore() : _io(), _workGuard(boost::asio::make_work_guard(_io))
{
}

AsioCore::~AsioCore()
{
    _workGuard.reset();
    _io.stop();
}

bool AsioCore::Register(std::shared_ptr<AsioObject> obj)
{
    if (!obj) return false;
    obj->Bind(_io);
    return true;
}

bool AsioCore::Dispatch(uint32_t timeoutMs)
{
    using namespace std::chrono;

    if (timeoutMs == INFINITE)
    {
        std::size_t handled = _io.run_one();
        return handled > 0;
    }
    else
    {
        const auto dur = milliseconds(timeoutMs);
        std::size_t handled = _io.run_for(dur);
        if (handled == 0)
            return false;
        return true;
    }
}