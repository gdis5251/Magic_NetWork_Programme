#include <jsoncpp/json/json.h>
#include "tcp_thread_server.hpp"

void CalServer(const std::string& req, std::string* resp)
{
    Json::Reader reader;
    Json::Value req_value;

    // 反序列化
    // 先把收到的字符串变成 JSON 格式
    reader.parse(req, req_value);

    // 然后把键值对中的值拿出来
    int num1 = req_value["num1"].asInt();
    int num2 = req_value["num2"].asInt();
    std::string op = req_value["op"].asString();

    // 计算结果
    int ret = 0;
    if (op == "+")
    {
        ret = num1 + num2;
    }
    else if (op == "-")
    {
        ret = num1 - num2;
    }

    // 序列化
    Json::FastWriter writer;
    Json::Value resp_value;

    resp_value["result"] = ret;
    *resp = writer.write(resp_value);
}

int main(void)
{
    TcpThreadServer server;
    std::string req;
    std::string resp;
    server.start("0.0.0.0", 9090, CalServer);

    return 0;
}