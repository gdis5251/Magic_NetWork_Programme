// 完成 epoll 接口的基本封装
// bool Init()
// bool Add(TcpSocket &sock)
// bool Del(TcpSocket &sock)
// bool Wait(std::vector<TcpSocket> &list, int timeout_msec)

#include <iostream>
#include <vector>
#include <sys/epoll.h>
#include "tcp_socket.hpp"

#define CHECK_RET(exp) if (!(exp)) {return false;}

const int MAX_EVENTS = 10;

class Epoll
{
public:
    bool Init()
    {
        epfd_ = epoll_create(1);
        if (epfd_ < 0)
        {
            perror("init");
            return false;
        }

        return true;
    }

    bool Add(TcpSocket &sock)
    {
        int fd = sock.GetFd();
        struct epoll_event ev;
        ev.data.fd = fd;
        ev.events = EPOLLIN;

        int ret = epoll_ctl(epfd_, EPOLL_CTL_ADD, fd, &ev);
        if (ret < 0)
        {
            perror("Add");
            return false;
        }

        return true;
    }

    bool Del(TcpSocket &sock)
    {
        int fd = sock.GetFd();
        int ret = (epfd_, EPOLL_CTL_DEL, fd, NULL);
        if (ret < 0)
        {
            perror("Del");
            return false;
        }

        return true;
    }

    bool Wait(std::vector<TcpSocket>& list, int timeout_msec = 3000)
    {
        struct epoll_event evs[MAX_EVENTS];
        int nfds = epoll_wait(epfd_, evs, MAX_EVENTS, timeout_msec);
        if (nfds < 0)
        {
            perror("Wait");
            return false;
        }
        if (nfds == 0)
        {
            std::cerr << "epoll wait timeout" << std::endl;
            return false;
        }

        for (int i = 0; i < nfds; i++)
        {
            TcpSocket sock;
            sock.SetFd(evs[i].data.fd);
            list.push_back(sock);
        }
        return true;
    }

private:
    int epfd_;
};

int main(void)
{
    TcpSocket lst_sock;
    CHECK_RET(lst_sock.Socket());
    CHECK_RET(lst_sock.Bind("0", 9090));
    CHECK_RET(lst_sock.Listen());

    Epoll epoll;
    CHECK_RET(epoll.Init());
    CHECK_RET(epoll.Add(lst_sock));

    while (1)
    {
        std::vector<TcpSocket> list;
        bool ret = epoll.Wait(list);
        if (!ret)
        {
            continue;
        }

        for (int i = 0; i < list.size(); i++)
        {
            if (lst_sock.GetFd() == list[i].GetFd())
            {
                TcpSocket cli_sock;
                std::string cli_ip;
                uint16_t cli_port;

                ret = lst_sock.Accept(&cli_sock, &cli_ip, &cli_port);
                if (ret < 0)
                {
                    continue;
                }

                epoll.Add(cli_sock);
            }
            else
            {
                std::string buf;
                int r = list[i].Recv(&buf);
                if (r <= 0)
                {
                    epoll.Del(list[i]);
                    list[i].Close();
                    std::cout << "对端关闭" << std::endl; 
                    continue;
                }
                std::cout << "Client Say: " << buf.c_str() << std::endl;
            }
        }
        

    }

    lst_sock.Close();
    return 0;
}