#pragma once

class AsioObject : public std::enable_shared_from_this<AsioObject>
{
public:
    virtual void Bind(boost::asio::io_context& io) = 0;
    virtual void Dispatch(void* eventOrNull, int32 numOfBytes = 0) {}
};

class AsioCore
{
public:
    AsioCore();
    ~AsioCore();

    boost::asio::io_context& GetContext() { return _io; }
    bool Register(std::shared_ptr<AsioObject> obj);
    bool Dispatch(uint32_t timeoutMs = INFINITE);

private:
    boost::asio::io_context _io;
    boost::asio::executor_work_guard<boost::asio::io_context::executor_type> _workGuard;
};
