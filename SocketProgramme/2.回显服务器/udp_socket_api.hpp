#pragma once
#include <iostream>
#include <cstdio>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

class UdpSocket
{
public:
    UdpSocket()
        :fd_(-1)
    {}

    bool Socket()
    {
        fd_ = socket(AF_INET, SOCK_DGRAM, 0);
        
        if (fd_ < 0)
        {
            perror("socket");
            return false;
        }

        return true;
    }

    bool Close()
    {
        if (fd_ != -1)
        {
            close(fd_);
        }
        return true;
    }


    // 将fd_ 与 指定端口号绑定
    bool Bind(const std::string& ip, uint16_t port)
    {
        sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = inet_addr(ip.c_str()); // inet_addr 是将点分式 ip 转换成结构体需要的格式
        addr.sin_port = htons(port); // htons 是将主机字节序转换成网络字节序

        int ret = bind(fd_, (sockaddr*)&addr, sizeof(addr));
        if (ret < 0)
        {
            perror("bind");
            return false;
        }

        return true;
    }

    // 三个输出型参数
    // 1. 接收到了什么请求
    // 2. 对方的 ip 地址
    // 3. 对方的端口号
    bool RecvFrom(std::string& msg, std::string* ip = nullptr, uint16_t *port = nullptr)
    {
        sockaddr_in peer;
        socklen_t len = sizeof(peer);

        char buf[10 * 1024] = {0};

        ssize_t num = recvfrom(fd_, buf, sizeof(buf) - 1, 0,
                               (sockaddr*)&peer, &len);
        if (num < 0)
        {
            perror("recvform");
            return false;
        }
        
        msg = buf;

        if (ip != nullptr)
        {
            *ip = inet_ntoa(peer.sin_addr);
        }
        if (port != nullptr)
        {
            *port = ntohs(peer.sin_port);
        }

        return true;
    }


    // 参数要求
    // 1. 发送的响应
    // 2. 对方的ip
    // 3. 对方的端口号
    bool SendTo(std::string& msg, std::string& ip, uint16_t port)
    {
        sockaddr_in peer;
        peer.sin_family = AF_INET;
        peer.sin_addr.s_addr = inet_addr(ip.c_str());
        peer.sin_port = htons(port);

        ssize_t num = sendto(fd_, msg.c_str(), msg.size(), 0,
                             (sockaddr*)&peer, sizeof(peer));
        if (num < 0)
        {
            perror("sendto");
            return false;
        }

        return true;
    }
private:
    int fd_;
};