#include <unordered_map>
#include "tcp_server.hpp"

int main()
{
    std::unordered_map<std::string, std::string> dict;
    dict.insert(std::make_pair("Hello", "你好"));
    dict.insert(std::make_pair("Gerald", "大帅哥"));
    dict.insert(std::make_pair("ALiBaBa", "阿里巴巴"));
    dict.insert(std::make_pair("Tencent", "腾讯"));
    dict.insert(std::make_pair("typedance", "字节跳动"));
    TcpServer server;
    server.start("0", 9090, [&dict](const std::string& req, std::string* resp)
                 {
                    auto it = dict.find(req);
                    if (it == dict.end())
                    {
                        *resp = "Can't Find!";
                    }
                    else
                    {
                        *resp = it->second;
                    }
                 });
    return 0;
}

