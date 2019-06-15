#pragma once
#include <iostream>
#include <cstdio>
#include <cassert>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>


class TcpSocket
{
public:
    TcpSocket()
        :fd_(-1)
    {}

    bool Socket()
    {
        fd_ = socket(AF_INET, SOCK_STREAM, 0);
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
            close(fd_);

        return true;
    }

    // 服务器用
    bool Bind(const std::string& ip, uint16_t port)
    {
        sockaddr_in addr;
        addr.sin_family = AF_INET;
        // addr.sin_addr.s_addr = inet_addr(ip.c_str());
        addr.sin_addr.s_addr = htons(INADDR_ANY);
        addr.sin_port = htons(port);

        int ret = bind(fd_, (sockaddr*)&addr, sizeof(addr));
        if (ret < 0)
        {
            perror("bind");
            return false;
        }

        return true;
    }

    // 服务器用
    bool Listen()
    {
        // 第一个参数·
        // 将socket标记成一个被动的 socket
        // 也就是把这个 socket 标记成一个服务器
        //
        // 参数 backlog 
        // 描述连接队列的长度
        // 队列和 socket 相关/ 每个调用 listen 的socket 有一个队列
        int ret = listen (fd_, 10);
        if (ret < 0)
        {
            perror("listen");
            return false;
        }
        return true;
    }

    // 服务器用
    bool Accept(TcpSocket* peer_socket, std::string* ip = nullptr, uint16_t* port = nullptr)
    {
        // 从连接队列中取一个连接到用户代码中
        // 如果队列中没有连接会阻塞(default action)
        
        // 服务器调用 accept
        // 获取对端 ip 和 端口号
        sockaddr_in peer;
        socklen_t len = sizeof(peer);

        int client_sock = accept(fd_, (sockaddr*)&peer, &len);
        if (client_sock < 0)
        {
            perror("accept");
            return false;
        }
        
        if (ip != nullptr)
        {
            *ip = inet_ntoa(peer.sin_addr);
        }
        if (port != nullptr)
        {
            *port = ntohs(peer.sin_port);
        }
        // 返回值也是一个 socket 
        // 是和客户端连接的 socket
        // 原 socket 继续接受连接
        // 返回的 socket 是处理和客户端的连接
        // 客户端有什么问题就跟这个 socket 联系

        peer_socket->fd_ = client_sock;
        return true;
    }


    int Recv(std::string* msg)
    {
        msg->clear();
        char buf[10 * 1024] = {0};

        ssize_t ret = recv(fd_, buf, sizeof(buf) - 1, 0);
        // 返回值
        // 如果成功返回读到的字节数
        // 失败了返回 -1
        // 对端结束，返回 0
        if (ret < 0)
        {
            perror("recv");
            return -1;
        }
        else if (ret == 0)
        {
            // 需要考虑返回0的情况
            return 0;
        }

        *msg = buf;
        return 1;
    }

    bool Send(const std::string& msg)
    {
        ssize_t ret = send(fd_, msg.c_str(), msg.size(), 0);
        if (ret < 0)
        {
            perror("send");
            return false;
        }

        return true;
    }

    // 客户端来用
    bool Connect(const std::string& ip, uint16_t port)
    {
        sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = inet_addr(ip.c_str());
        addr.sin_port = htons(port);

        int ret = connect(fd_, (sockaddr*)&addr, sizeof(addr));
        if (ret < 0)
        {
            perror("connect");
            return false;
        }
        return true;
    }
private:
    int fd_;
};
