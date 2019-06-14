#pragma once
#include "tcp_socket.hpp"
#include <functional>

#define CHECK_RET(exp) if (!(exp)) { return false; }

typedef std::function<void(const std::string&, std::string*)> Handler;

class TcpServer
{
public:

    bool start(const std::string& ip, uint16_t port, Handler handler)
    {
        // 1. 创建一个 socket
        CHECK_RET(listen_sock_.Socket());

        // 2. 绑定端口号
        CHECK_RET(listen_sock_.Bind(ip, port));

        // 3. 进行监听
        CHECK_RET(listen_sock_.Listen());
        std::cout << "Server Start OK!" << std::endl;

        // 4. 进入主循环
        while (true)
        {
            // 5. 获取连接
            TcpSocket client_socket;
            std::string ip;
            uint16_t port;

            bool ret = listen_sock_.Accept(&client_socket, &ip, &port);
            if (ret < 0)
            {
                continue;
            }
            printf("[%s][%d]连接成功", ip.c_str(), port );

            // 6. 和客户端进行具体的沟通
            // 一次连接中和客户端进行多次连接
            while (true)
            {
                // a. 读取请求
                std::string req;
                int r = client_socket.RecvFrom(&req);

                if (r < 0)
                {
                    continue;
                }
                if (r == 0)
                {
                    // 对端关闭了 socket
                    client_socket.Close();
                    printf("[%s][%d]对端关闭", ip.c_str(), port );
                    break;
                }
                printf("[%s][%d]: %s", ip.c_str(), port, req.c_str());

                // b. 根据请求计算响应
                std::string resp;
                handler(req, &resp);

                // c. 把响应写会客户端
                client_socket.Send(resp);
            }       
            
        }

    }
private:
   TcpSocket listen_sock_; 
};

