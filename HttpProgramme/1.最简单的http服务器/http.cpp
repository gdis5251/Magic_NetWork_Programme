// 实现一个最简单的 HTTP 服务器
// HTTP 服务器实际是一个 TCP 服务器，不过是应用数据处理是 HTTP 协议格式
// 处理数据的时候：
//   1. 获取 HTTP 头部（首行 + 头部）
//   2. 获取数据
//   3. 根据头部中的请求处理数据
// 处理方式：接收到浏览器的请求之后，打印请求的数据，并且响应：
// <html><body><h1>Hello World</h1></body></html>
#include <sstream>
#include "tcp_thread_server.hpp"

int main(void)
{
    TcpThreadServer server;
    server.start("0", 9090, [](const std::string& req, std::string* resp, 
                    TcpSocket client_sock)
    {
        std::string first_line("HTTP/1.1 200 OK\r\n");
        std::string body("<html><body><h1>I Love You.</h1></body></html>");
       
        std::stringstream header;
        header << "Content-Length: " << body.size() << "\r\n";
        header << "Content-Type: " << "text/html\r\n";

        std::string blank("\r\n");

        client_sock.Send(first_line);
        client_sock.Send(header.str());
        client_sock.Send(blank);
        client_sock.Send(body);
    });
    return 0;
}