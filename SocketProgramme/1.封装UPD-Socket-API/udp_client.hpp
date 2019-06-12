#pragma once
#include "upd_socket_api.hpp"

class UdpClient
{
public:
    UdpClient(const std::string& ip, const uint16_t port)
        :server_ip_(ip),
        server_port_(port)
    {
        sock_.Socket();
    }

    ~UdpClient()
    {
        sock_.Close();
    }

    bool RecvFrom(std::string& msg)
    {
        return sock_.RecvFrom(msg);
    }

    bool SendTo(std::string& msg)
    {
        return sock_.SendTo(msg, server_ip_, server_port_);
    }
private:
    UdpSocket sock_;
    std::string server_ip_;
    uint16_t server_port_;
};
