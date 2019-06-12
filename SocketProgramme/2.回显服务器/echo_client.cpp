#include "udp_client.hpp"

int main(int argc, char* argv[])
{
    (void)argc;
    UdpClient client(argv[1], 9090);

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
