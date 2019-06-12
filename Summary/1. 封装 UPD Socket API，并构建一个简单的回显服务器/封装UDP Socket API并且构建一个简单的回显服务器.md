# 封装UDP Socket API并且构建一个简单的回显服务器

> 因为 Socket API 用起来非常的不便利，所以我将把 Socket API 封装起来，让我以后写程序的时候不再关注那些不重要的细节，而是把我的关注点放在接收到请求之后，怎么处理这个请求上。

## udp_socket_api.hpp

```cpp
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
```

## udp_server.hpp

这里封装的是服务器的处理行为。以后需要写一个 UDP 服务器直接在这里使用接口就行。

```cpp
#pragma once
#include <cassert>
#include <functional>
#include "upd_socket_api.hpp"

// 一个可以接收函数指针和仿函数的通用的东西
typedef std::function<void(const std::string&, std::string&)> Handler;

class UdpServer
{
public:
    UdpServer() 
    {
        sock_.Socket();
    }
    ~UdpServer() 
    {
        sock_.Close();
    }

    bool start(const std::string& ip, uint16_t port, Handler handler)
    {
        // 先绑定
        int ret = sock_.Bind(ip, port);
        if (!ret)
        {
            perror("Bind");
            return false;
        }
        
        while (true)
        {
            std::string req;
            std::string peer_ip;
            uint16_t peer_port;
            // 1. 接收请求
            ret = sock_.RecvFrom(req, &peer_ip, &peer_port);
            if (!ret)
            {
                perror("RecvFrom");
                continue;
            }
            // 打印接收到的信息
            printf("[ip: %s --- port: %d]  %s\n", peer_ip.c_str(), peer_port, req.c_str());

            // 2. 处理请求
            std::string resp;
            handler(req, resp);

            // 3. 响应请求
            sock_.SendTo(resp, peer_ip, peer_port);
        }
    }

private:
    UdpSocket sock_;
};
```

## udp_client.hpp

同样这里是封装了客户端的操作，以后写客户端也就直接在这里使用接口。

```cpp
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
```

# 回显服务器

上面封装完那些接口之后，这里在需要写一个服务器和客户端就非常简单了。

### 什么是回显服务器？

就是你输入什么，就在屏幕上打印你所输入的话。

这是一个最简单的服务器。相当于我们刚开始学 C 语言的时候，学习怎么输出 Hello World!

## echo_server.cpp

```cpp
#include "udp_server.hpp"

int main()
{
    UdpServer server;

    server.start("0", 9090, [](const std::string& req, std::string& resp)
                 {
                    resp = req;
                 });
    return 0;
}
```

## echo_client.cpp

```cpp
#include "udp_client.hpp"

int main()
{
    UdpClient client("47.101.192.120", 9090);

    std::cout << "欢迎使用回显服务器！" << std::endl;
    while (true)
    {
        std::cout << "请输入一段内容：";
        fflush(stdout);

        std::string req;
        std::getline(std::cin, req);

        client.SendTo(req);

        std::string resp;
        client.RecvFrom(resp);
        
        std::cout << resp.c_str() << std::endl;
    }
    return 0;
}
```

是不是感觉代码也太少了！写个服务器就这么点代码？

正是因为封装了及其复杂的 socket api 所以，我们在写服务器的时候，只需要好好想怎么写处理请求的仿函数就行，这样我们的关注点就可以放到真正的核心部分。

## 演示：

首先我将在我的阿里云服务器上启动服务。

![在这里插入图片描述](https://img-blog.csdnimg.cn/2019061215542210.png)

然后再在我自己的虚拟机上启动客户端程序。

![在这里插入图片描述](https://img-blog.csdnimg.cn/20190612155431132.png)

在客户端上输入一句话。

![在这里插入图片描述](https://img-blog.csdnimg.cn/2019061215543856.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3dlaXhpbl80MjY3ODUwNw==,size_16,color_FFFFFF,t_70)

服务器显示（这个根据你自己的处理，你也可以不让服务器显示）。

![在这里插入图片描述](https://img-blog.csdnimg.cn/20190612155442884.png)



叮~:bell: