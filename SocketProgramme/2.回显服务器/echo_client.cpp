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
