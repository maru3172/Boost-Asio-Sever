#include "pch.h"
#include "Listener.h"
#include "Service.h"
#include "Session.h"

using boost::asio::ip::tcp;

Listener::~Listener()
{
    CloseSocket();
    _pending.clear();
}

void Listener::Bind(boost::asio::io_context& io)
{
    _io = &io;
}

bool Listener::StartAccept(ServerServiceRef service)
{
    _service = service;
    if (!_service || !_io) return false;

    if (!_acceptor) _acceptor = std::make_unique<tcp::acceptor>(*_io);
    if (_acceptor->is_open()) {
        boost::system::error_code ec;
        _acceptor->close(ec);
    }

    _acceptor->open(tcp::v4());
    _acceptor->set_option(boost::asio::socket_base::reuse_address(true));
    _acceptor->set_option(boost::asio::socket_base::linger(true, 0));

    auto ep = _service->GetNetAddress().GetEndpoint();
    _acceptor->bind(ep);
    _acceptor->listen();

    const int32 acceptCount = _service->GetMaxSessionCount();
    _pending.resize(acceptCount);
    for (int32 i = 0; i < acceptCount; ++i)
        PostAccept(static_cast<size_t>(i));

    return true;
}

void Listener::CloseSocket()
{
    if (_acceptor && _acceptor->is_open()) {
        boost::system::error_code ec;
        _acceptor->close(ec);
    }
}

void Listener::PostAccept(size_t idx)
{
    SessionRef session = _service->CreateSession();
    _pending[idx] = session;

    _acceptor->async_accept(session->GetSocket(),
        [this, self = std::static_pointer_cast<Listener>(shared_from_this()),
        idx, session](const boost::system::error_code& ec)
        {
            if (!ec)
            {
                boost::system::error_code ep_ec;
                auto rep = session->GetSocket().remote_endpoint(ep_ec);
                if (!ep_ec) {
                    session->SetNetAddress(NetAddress(rep));
                }
                session->ProcessConnect();
            }
            PostAccept(idx);
        });
}