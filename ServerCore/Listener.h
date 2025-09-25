#pragma once
#include "AsioCore.h"
#include "NetAddress.h"

class ServerService;
class Session;

class Listener : public AsioObject
{
protected:
    boost::asio::io_context* _io = nullptr;
    std::unique_ptr<boost::asio::ip::tcp::acceptor> _acceptor;
    ServerServiceRef _service;
    Vector<SessionRef> _pending;

public:
    Listener() = default;
    ~Listener();

    bool StartAccept(ServerServiceRef service);
    void CloseSocket();

    void Bind(boost::asio::io_context& io) override;
    void Dispatch(void* eventOrNull, int32 numOfBytes = 0) override {}

private:
    void PostAccept(size_t idx);
};
