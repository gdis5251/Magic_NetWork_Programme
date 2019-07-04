#include <sys/select.h>
#include <vector>
#include <unistd.h>
#include "tcp_socket.hpp"

#define CHECK_RET(exp) if (!(exp)) {return false;}

class Select
{
public:
    Select()
        :max_fd_(-1)
    {
        FD_ZERO(&rfds_); // 清空描述符集合
    }
    // 将用户关心 socket 描述符添加到监控集合中
    bool Add(TcpSocket &sock)
    {
        int fd = sock.GetFd();
        // 向 set 描述符集合中添加 fd 描述符
        FD_SET(fd, &rfds_);

        max_fd_ = max_fd_ > fd ? max_fd_ : fd;

        return true;
    }

    bool Del(TcpSocket &sock)
    {
        int fd = sock.GetFd();

        // 从 set 描述符集合中移除 fd 描述符
        FD_CLR(fd, &rfds_);

        for (int i = max_fd_; i >= 0; i--)
        {
            // 判断 fd 描述符是否还在 set 集合中
            if (FD_ISSET(i, &rfds_))
            {
                max_fd_ = i;
                break;
            }
        }
        return true;
    }
    bool wait(std::vector<TcpSocket> &list, int timeout_sec = 3)
    {
        struct timeval tv;
        tv.tv_sec = timeout_sec;
        tv.tv_usec = 0;
        fd_set set = rfds_;

        int ret = select(max_fd_ + 1, &set, nullptr, nullptr, &tv);
        if (ret < 0)
        {
            perror("select");
            return false;
        }
        else if (ret == 0)
        {
            std::cout << "Select wait timeout !" << std::endl;
            return false;
        }

        for (int i = 0; i <= max_fd_; i++)
        {
            if (FD_ISSET(i, &set))
            {
                TcpSocket sock;
                sock.SetFd(i);
                list.push_back(sock);
            }
        }
        return true;
    }

private:
    fd_set rfds_;
    int max_fd_;
};

int main(void)
{
    TcpSocket sock;
    CHECK_RET(sock.Socket());
    CHECK_RET(sock.Bind("0", 9090));
    CHECK_RET(sock.Listen());

    Select s;
    s.Add(sock);
    while (true)
    {
        std::vector<TcpSocket> list;
        if (s.wait(list) == false)
        {
            continue;
        }
        for (int i = 0; i < list.size(); i++)
        {
            if (list[i].GetFd() == sock.GetFd())
            {

                TcpSocket client_sock;
                std::string cli_ip;
                uint16_t cli_port;

                if (sock.Accept(&client_sock, &cli_ip, &cli_port) == false)
                {
                    continue;
                }
                s.Add(client_sock);
            }
            else
            {
                std::string buf;
                if (list[i].Recv(&buf) == false)
                {
                    s.Del(list[i]);
                    list[i].Close();
                    continue;
                }
                std::cout << "Recv:" << buf.c_str() << std::endl;
            }
        }

    }

    return 0;
}
