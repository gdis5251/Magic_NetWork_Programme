#pragma once
#include <functional>
#include <vector>
#include <unistd.h>
#include <sys/epoll.h>
#include "tcp_socket.hpp"

bool operator<(const TcpSocket& lhs, const TcpSocket& rhs)
{
    return lhs.GetFd() < rhs.GetFd();
}

class Epoll
{
public:
    Epoll()
    {
        epoll_fd_ = epoll_create(1);
    }

    ~Epoll()
    {
        close(epoll_fd_);
    }
    void Add(const TcpSocket& sock)
    {
        printf("[Epoll:Add] %d\n", sock.GetFd());
        epoll_event event;
        event.events = EPOLLIN;

        // 此处 epoll add 的时候插入的是键值对
        // fd 在 键和值之间都出现了，这件事情是迫不得已
        event.data.fd = sock.GetFd();
        epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, sock.GetFd(), &event);
    }

    void Del(const TcpSocket& sock)
    {
        printf("[Epoll:Add] %d\n", sock.GetFd());
        epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, sock.GetFd(), nullptr);
    }

    void Wait(std::vector<TcpSocket>* output)
    {
        output->clear();
        epoll_event events[100];
        // 返回值结果就是在描述数组中有几个有效元素
        // epoll_wait 返回的内容只是键值对的值
        // 如果不加任何处理的话，用户不知道对应的文件描述符是谁
        // 迫不得已，只能在插入的时候，把 socket 往值里也存一份
        int nfds = epoll_wait(epoll_fd_, events, 100, -1);
        if (nfds < 0)
        {
            perror("epoll_wait");
            return;
        }

        for (int i = 0; i < nfds; i++)
        {
            output->push_back(TcpSocket(events[i].data.fd));
        }
    }

private:
    int epoll_fd_;
};

typedef std::function<void (const std::string&, std::string*)> Handler;

#define CHECK_RET(exp) \
    if (!exp) \
    {\
        return false;\
    }

// 实现一个 Select 版本的 TCP server
class TcpEpollServer
{
public:
    bool Start(const std::string& ip, uint16_t port,
               Handler handler)
    {
        // 创建 socket
        TcpSocket listen_socket;
        CHECK_RET(listen_socket.Socket());
        CHECK_RET(listen_socket.Bind(ip, port));
        CHECK_RET(listen_socket.Listen());

        // 创建一个 epoll 对象把 listen_socket 管理起来
        Epoll epoll;
        epoll.Add(listen_socket);

        
        // 进入主循环
        while (true)
        {
            // 不再是直接调用 accept
            // 而是用 epoll 进行多路复用
            std::vector<TcpSocket> output;
            epoll.Wait(&output);

            // 遍历返回结果，一次处理每个就绪的 socket
            for (auto& tcp_socket : output)
            {
                // 分成两种情况讨论
                //   a) 如果就绪的是 listen_socket 调用 accept
                if (tcp_socket.GetFd() == listen_socket.GetFd())
                {
                    TcpSocket client_socket;
                    std::string ip;
                    uint16_t port;
                    tcp_socket.Accept(&client_socket, &ip, &port);
                    printf("[client %s:%d] connected\n", ip.c_str(), port);

                    epoll.Add(client_socket);
                }
                //   b) 如果就绪的 socket 不是 listen_socket 调用 recv
                else
                {
                    std::string req;
                    int ret = tcp_socket.Recv(&req);
                    if (ret < 0)
                    {
                        continue;
                    }
                    else if (0 == ret)
                    {
                        printf("对端关闭！\n");
                        epoll.Del(tcp_socket);
                        tcp_socket.Close();
                        continue;
                    }

                    printf("[client] %s\n", req.c_str());

                    std::string resp;
                    // 根据请求完成响应的过程
                    handler(req, &resp);
                    tcp_socket.Send(resp);
                }
            }
        }
    }
private:
};
