#pragma once
#include <vector>
// 需要尽量高效的插入和删除
// 需要方便的找到最大元素
#include <set>
#include <string>
#include <functional>
#include <sys/select.h>
#include "tcp_socket.hpp"

bool operator<(const TcpSocket& lhs, const TcpSocket& rhs)
{
    return lhs.GetFd() < rhs.GetFd();
}

class Selector
{
public:
    void Add (const TcpSocket& sock)
    {
        printf("[Selector::Add] %d\n", sock.GetFd());
        socket_set_.insert(sock);
    }

    void Del (const TcpSocket& sock)
    {
        printf("[Selector::Del] %d\n", sock.GetFd());
        socket_set_.erase(sock);
    }

    // Wait 返回的时候要告诉用户哪个文件描述符就绪了
    void Wait(std::vector<TcpSocket>* output)
    {
        // 调用 Wait 就相当于调用 select 进行等待
        // 先获取到最大文件描述符
        
        if (socket_set_.empty())
        {
            printf("[Selector::Wait] socket_set_ is empty\n");
            return;
        }
        int max_fd = socket_set_.rbegin()->GetFd();
        fd_set readfds;
        FD_ZERO(&readfds);
        // 把每个关注的 fd 都添加到 readfds 中
        for (const auto& sock : socket_set_)
        {
            FD_SET(sock.GetFd(), &readfds);
        }
        // 默认是阻塞等待, 有文件描述符就绪的时候，才会返回
        // 当函数返回后，根据函数的返回情况构造输出参数，告诉调用者哪些描述符就绪了
        int nfds = select(max_fd + 1, &readfds, nullptr, nullptr, nullptr);
        if (nfds < 0)
        {
            perror("Select");
            return;
        }

        for (int fd = 0; fd < max_fd + 1; fd++)
        {
            if (FD_ISSET(fd, &readfds))
            {
                output->push_back(TcpSocket(fd));
            }
        }

    }

private:
    // 用一个数据结构把文件描述符存起来
    // 要求 TCPSocket 类能支持比较操作 operator<
    std::set<TcpSocket> socket_set_;
};

typedef std::function<void (const std::string&, std::string*)> Handler;

#define CHECK_RET(exp) \
    if (!exp) \
    {\
        return false;\
    }

// 实现一个 Select 版本的 TCP server
class TcpSelectServer
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

        // 创建一个 select 对象把 listen_socket 管理起来
        Selector selector;
        selector.Add(listen_socket);

        // 后续进行等待都是靠 selector 对象完成
        
        // 进入主循环
        while (true)
        {
            // 不再是直接调用 accept
            // 而是用 seletor 进行多路复用
            std::vector<TcpSocket> output;
            selector.Wait(&output);

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

                    selector.Add(client_socket);
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
                        selector.Del(tcp_socket);
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
