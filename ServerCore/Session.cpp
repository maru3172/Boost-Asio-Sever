#include "pch.h"
#include "Session.h"
#include "Service.h"

using boost::asio::ip::tcp;

Session::Session()
    : _recvBuffer(BUFFER_SIZE)
{
}

Session::~Session()
{
    boost::system::error_code ec;
    if (_socket && _socket->is_open())
    {
        _socket->shutdown(tcp::socket::shutdown_both, ec);
        _socket->close(ec);
    }
}

void Session::Bind(boost::asio::io_context& io)
{
    _io = &io;
    _socket = std::make_unique<tcp::socket>(io);
}

void Session::Send(SendBufferRef sendBuffer)
{
    if (IsConnected() == false)
        return;

    bool registerSend = false;

    {
        WRITE_LOCK;

        _sendQueue.push(sendBuffer);

        if (_sendRegistered.exchange(true) == false)
            registerSend = true;
    }

    if (registerSend)
        RegisterSend();
}

bool Session::Connect()
{
    return RegisterConnect();
}

void Session::Disconnect(const WCHAR* cause)
{
    if (_connected.exchange(false) == false)
        return;

    std::wcout << L"Disconnect : " << cause << std::endl;

    RegisterDisconnect();
}

bool Session::RegisterConnect()
{
    if (IsConnected())
        return false;

    if (GetService()->GetServiceType() != ServiceType::Client)
        return false;

    auto ep = GetService()->GetNetAddress().GetEndpoint();

    _socket->async_connect(ep,
        [self = GetSessionRef()](const boost::system::error_code& ec)
        {
            if (!ec)
                self->ProcessConnect();
            else
                self->HandleError(static_cast<int32>(ec.value()));
        });

    return true;
}

bool Session::RegisterDisconnect()
{
    if (!_io) return false;

    boost::system::error_code ec1;
    _socket->shutdown(tcp::socket::shutdown_both, ec1);

    boost::asio::post(*_io, [self = GetSessionRef()]() { self->ProcessDisconnect(); });

    return true;
}

void Session::RegisterRecv()
{
    if (IsConnected() == false)
        return;

    auto bufPtr = _recvBuffer.WritePos();
    auto len = _recvBuffer.FreeSize();

    _socket->async_read_some(boost::asio::buffer(bufPtr, len),
        [self = GetSessionRef()](const boost::system::error_code& ec, std::size_t bytes)
        {
            if (!ec)
                self->ProcessRecv(static_cast<int32>(bytes));
            else
                self->HandleError(static_cast<int32>(ec.value()));
        });
}

void Session::RegisterSend()
{
    if (IsConnected() == false)
        return;

    Vector<SendBufferRef> localBuffers;

    {
        WRITE_LOCK;

        while (_sendQueue.empty() == false)
        {
            localBuffers.push_back(_sendQueue.front());
            _sendQueue.pop();
        }
    }

    if (localBuffers.empty())
    {
        _sendRegistered.store(false);
        return;
    }

    Vector<boost::asio::const_buffer> bufs;
    bufs.reserve(localBuffers.size());
    for (auto& sb : localBuffers)
        bufs.push_back(boost::asio::buffer(sb->Buffer(), sb->WriteSize()));

    _socket->async_write_some(bufs,
        [self = GetSessionRef(), localBuffers, this](const boost::system::error_code& ec, std::size_t bytes)
        {
            if (!ec)
                self->ProcessSend(static_cast<int32>(bytes));
            else
            {
                self->HandleError(static_cast<int32>(ec.value()));
                _sendRegistered.store(false);
            }
        });
}

void Session::ProcessConnect()
{
    _connected.store(true);

    GetService()->AddSession(GetSessionRef());

    OnConnected();

    RegisterRecv();
}

void Session::ProcessDisconnect()
{
    OnDisconnected();
    GetService()->ReleaseSession(GetSessionRef());

    boost::system::error_code ec;
    if (_socket->is_open())
        _socket->close(ec);
}

void Session::ProcessRecv(int32 numOfBytes)
{
    if (numOfBytes == 0)
    {
        Disconnect(L"Recv 0");
        return;
    }

    if (_recvBuffer.OnWrite(numOfBytes) == false)
    {
        Disconnect(L"OnWrite Overflow");
        return;
    }

    int32 dataSize = _recvBuffer.DataSize();
    int32 processLen = OnRecv(_recvBuffer.ReadPos(), dataSize);
    if (processLen < 0 || dataSize < processLen || _recvBuffer.OnRead(processLen) == false)
    {
        Disconnect(L"OnRead Overflow");
        return;
    }

    _recvBuffer.Clean();

    RegisterRecv();
}

void Session::ProcessSend(int32 numOfBytes)
{
    if (numOfBytes == 0)
    {
        Disconnect(L"Send 0");
        return;
    }

    OnSend(numOfBytes);

    WRITE_LOCK;
    if (_sendQueue.empty())
        _sendRegistered.store(false);
    else
        RegisterSend();
}

void Session::HandleError(int32 errorCode)
{
    switch (errorCode)
    {
    case 10054:
    case 10053:
        Disconnect(L"HandleError");
        break;
    default:
        std::cout << "Handle Error : " << errorCode << std::endl;
        break;
    }
}

PacketSession::PacketSession()
{
}

PacketSession::~PacketSession()
{
}

int32 PacketSession::OnRecv(BYTE* buffer, int32 len)
{
    int32 processLen = 0;

    while (true)
    {
        int32 dataSize = len - processLen;

        if (dataSize < sizeof(PacketHeader))
            break;

        PacketHeader header = *(reinterpret_cast<PacketHeader*>(&buffer[processLen]));
        if (dataSize < header.size)
            break;

        OnRecvPacket(&buffer[processLen], header.size);

        processLen += header.size;
    }

    return processLen;
}