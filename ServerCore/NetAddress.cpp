#include "pch.h"
#include "NetAddress.h"

using boost::asio::ip::tcp;
using boost::asio::ip::address_v4;

static std::string NarrowFromWide(const std::wstring& ws)
{
    std::string s;
    s.reserve(ws.size());
    for (wchar_t c : ws) s.push_back(static_cast<char>(c));
    return s;
}

NetAddress::NetAddress(tcp::endpoint ep) : _endpoint(std::move(ep))
{
}

NetAddress::NetAddress(std::wstring ip, uint16_t port)
{
    auto addr = ip2Address(ip.c_str());
    _endpoint = tcp::endpoint(addr, port);
}

std::wstring NetAddress::GetIpAddress()
{
    auto s = _endpoint.address().to_string();
    return std::wstring(s.begin(), s.end());
}

address_v4 NetAddress::ip2Address(const wchar_t* ip)
{
    std::wstring ws(ip ? ip : L"");
    auto ns = NarrowFromWide(ws);
    return boost::asio::ip::make_address_v4(ns);
}