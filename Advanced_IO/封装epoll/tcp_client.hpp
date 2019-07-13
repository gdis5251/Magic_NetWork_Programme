#pragma once
#include "tcp_socket.hpp"

class TcpClient
{
public:
    TcpClient()
    {
        sock_.Socket();
    }

    ~TcpClient()
    {
        sock_.Close();
    }

    bool Connect(const std::string& ip = "127.0.0.1", uint16_t port = 9090)
    {
        sock_.Connect(ip, port);
    }

    int Recv(std::string* msg)
    {
        return sock_.Recv(msg);
    }

    bool Send(const std::string& resp)
    {
        return sock_.Send(resp);
    }
private:
    TcpSocket sock_;
};
