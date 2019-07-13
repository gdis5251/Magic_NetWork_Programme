#include <vector>
#include <jsoncpp/json/json.h>
#include "select_tcp_server.hpp"

void CalServer(const std::string& req, std::string* resp)
{
    Json::Reader reader;
    Json::Value req_value;

    // 反序列化
    // 先把收到的字符串变成 JSON 格式
    reader.parse(req, req_value);

    // 然后把键值对中的值拿出来
    std::vector<int> nums;
    for (int i = 0; i < req_value["nums"].size(); i++)
    {
        nums.push_back(req_value["nums"][i].asInt());
    }
    std::string op = req_value["op"].asString();

    // 计算结果
    int ret = 0;
    if (op == "+")
    {
        ret = nums[0] + nums[1];
    }
    else if (op == "-")
    {
        ret = nums[0] - nums[1];
    }

    // 序列化
    Json::FastWriter writer;
    Json::Value resp_value;

    resp_value["result"] = ret;
    *resp = writer.write(resp_value);
}

int main(void)
{
    TcpSelectServer server;
    std::string req;
    std::string resp;
    server.Start("0.0.0.0", 9090, CalServer);

    return 0;
}
