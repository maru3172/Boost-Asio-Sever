#pragma once

class NetAddress
{
    boost::asio::ip::tcp::endpoint _endpoint;

public:
    NetAddress() = default;
    NetAddress(boost::asio::ip::tcp::endpoint ep);
    NetAddress(std::wstring ip, uint16_t port);

    boost::asio::ip::tcp::endpoint& GetEndpoint() { return _endpoint; }
    std::wstring GetIpAddress();
    uint16_t GetPort() { return static_cast<uint16_t>(_endpoint.port()); }

    static boost::asio::ip::address_v4 ip2Address(const wchar_t* ip);
};
