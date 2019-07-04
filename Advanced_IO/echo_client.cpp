#include "tcp_client.hpp"

int main()
{
    TcpClient client;

    client.Connect();
    std::cout << "欢迎使用回显服务器！" << std::endl;
    while (true)
    {
        std::cout << "请输入一段内容：";
        fflush(stdout);

        std::string req;
        std::getline(std::cin, req);

        client.Send(req);
    }
    return 0;
}
