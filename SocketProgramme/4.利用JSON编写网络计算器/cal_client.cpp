#include <jsoncpp/json/json.h>
#include "tcp_client.hpp"

int main(void)
{
    TcpClient client;
    client.Connect();
    
    int num1 = 0;
    int num2 = 0;
    std::string op;

    while (std::cin >> num1 >> num2 >> op)
    {
        // 序列化
        Json::Value req_value;
        Json::StyledWriter writer;

        req_value["num1"] = num1;
        req_value["num2"] = num2;
        req_value["op"] = op;

        std::string req = writer.write(req_value);

        // 发送
        client.Send(req);

        // 接收
        std::string resp;
        client.Recv(&resp);

        // 反序列化
        Json::Reader reader;
        Json::Value resp_value;

        reader.parse(resp, resp_value);

        int result = resp_value["result"].asInt();

        std::cout << "Result: " << result << std::endl;
    }
    return 0;
}