#pragma once
#include "tcp_socket.hpp"
#include <functional>
#include <pthread.h>

typedef std::function<void (const std::string& req, std::string* resp,
                                TcpSocket client_sock)> Handler;

#define CHECK_RET(exp) if (!(exp)) {return false;}

class TcpThreadServer
{
public:
    TcpThreadServer()
    {

    }
    ~TcpThreadServer()
    {
        listen_sock_.Close();
    }

    bool start(const std::string&ip, uint16_t port, Handler handler)
    {
        CHECK_RET(listen_sock_.Socket());

        CHECK_RET(listen_sock_.Bind(ip, port));

        CHECK_RET(listen_sock_.Listen());

        while (true)
        {
            // 1. 获取连接
            TcpSocket client_sock;
            std::string peer_ip;
            uint16_t peer_port;

            int ret = listen_sock_.Accept(&client_sock, &peer_ip, &peer_port); 
            if (ret < 0)
            {
                continue;
            }

            // 2. 创建线程
            ThreadArg* arg = new ThreadArg;
            arg->client_sock_ = client_sock;
            arg->ip_ = ip;
            arg->port_ = port;
            arg->handler_ = handler;

            pthread_t tid;
            ret = pthread_create(&tid, nullptr, ThreadEntry, (void*)arg);           

            // 3. 主线程继续获取连接
            pthread_detach(tid);
        }
    }
private:
    TcpSocket listen_sock_;

    struct ThreadArg
    {
        TcpSocket client_sock_;
        std::string ip_;
        uint16_t port_;
        Handler handler_;
    };

    static void* ThreadEntry(void *arg)
    {
        while (true)
        {
            ThreadArg* thread_arg = (ThreadArg*)arg;
            // 1. 接收请求
            std::string req;
            int r = thread_arg->client_sock_.Recv(&req);
            if (r < 0)
            {
                // 接收失败
                continue;
            }
            if (r == 0)
            {
                printf("[%s : %d] 对端关闭！\n", thread_arg->ip_.c_str(), thread_arg->port_);
                thread_arg->client_sock_.Close();
                break;
            }
            // 走到这里说明接收消息成功
            printf("[%s : %d] 对端发送了： %s\n", thread_arg->ip_.c_str(), thread_arg->port_, req.c_str());

            // 2. 线程处理请求
            std::string resp;
            thread_arg->handler_(req, &resp, thread_arg->client_sock_);

            // 3. 将响应结果发送回客户端
            // thread_arg->client_sock_.Send(resp);
        }
    }
};
