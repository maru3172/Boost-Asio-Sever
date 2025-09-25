#pragma once
#include "AsioCore.h"
#include "NetAddress.h"
#include "RecvBuffer.h"

class Service;

class Session : public AsioObject
{
public:
    Session();
    virtual ~Session();

    void Send(SendBufferRef sendBuffer);
    bool Connect();
    void Disconnect(const WCHAR* cause);

    std::shared_ptr<Service> GetService() { return _service.lock(); }
    void SetService(std::shared_ptr<Service> service) { _service = service; }

    void SetNetAddress(NetAddress address) { _netAddress = address; }
    NetAddress GetAddress() { return _netAddress; }
    boost::asio::ip::tcp::socket& GetSocket() { return *_socket; }
    bool IsConnected() { return _connected; }
    SessionRef GetSessionRef() { return std::static_pointer_cast<Session>(shared_from_this()); }

protected:
    virtual void OnConnected() {}
    virtual int32 OnRecv(BYTE* buffer, int32 len) { return len; }
    virtual void OnSend(int32 len) {}
    virtual void OnDisconnected() {}

private:
    friend class Listener;
    friend class AsioCore;
    friend class Service;

    enum
    {
        BUFFER_SIZE = 0x10000,
    };

    std::weak_ptr<Service> _service;
    boost::asio::io_context* _io = nullptr;
    std::unique_ptr<boost::asio::ip::tcp::socket> _socket;
    NetAddress _netAddress;
    Atomic<bool> _connected = false;

    USE_LOCK;

    RecvBuffer _recvBuffer;
    Queue<SendBufferRef> _sendQueue;
    Atomic<bool> _sendRegistered = false;

    void Bind(boost::asio::io_context& io) override;
    void Dispatch(void* eventOrNull, int32 numOfBytes = 0) override {}

    bool RegisterConnect();
    bool RegisterDisconnect();
    void RegisterRecv();
    void RegisterSend();

    void ProcessConnect();
    void ProcessDisconnect();
    void ProcessRecv(int32 numOfBytes);
    void ProcessSend(int32 numOfBytes);

    void HandleError(int32 errorCode);
};

struct PacketHeader
{
	uint16 size;
	uint16 id;
};

class PacketSession : public Session
{
protected:
    virtual int32 OnRecv(BYTE* buffer, int32 len) sealed;
    virtual void OnRecvPacket(BYTE* buffer, int32 len) abstract;
public:
    PacketSession();
    virtual ~PacketSession();

    PacketSessionRef GetPacketSessionRef() { return std::static_pointer_cast<PacketSession>(shared_from_this()); }
};
