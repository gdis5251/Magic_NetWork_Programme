#include "tcp_client.hpp"

int main(int argc, char* argv[])
{
    // if (argc < 2)
    // {
    //     std::cout << "error input!" << std::endl;
    //     std::cout << "You should: ./server xxx.xxx.xxx.xxx";
    //     return 1;
    // }
    TcpClient client;
    int ret = client.Connect();
    if (ret < 0)
    {
        std::cout << "Connect Failed !" << std::endl;
        return 1;
    }

    while (true)
    {
        // 从标准输入读数据
        std::cout << "请输入要查询的单词: ";
        fflush(stdout);

        char req[1024] = {0};
        std::cin >> req;

        // 把读到的数据发给服务器
        client.Send(req);
        // 读取服务器的响应结果
        std::string resp;
        client.Recv(&resp);
        // 把响应结果显示到输出上
        std::cout << resp.c_str() << std::endl;
    }

    return 0;
}

