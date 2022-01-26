#include <iostream>
#include <unistd.h>
#include <sys/time.h>
#include <sys/select.h>
#include <cstring>
/*
多进程的方式会导致随着客户端连接数的上升，程序所占内存和资源会急剧变大，
这很明显对于服务器来说不是一个很好地解决办法。
可以采用 I/O 复用的方式实现并发服务器的操作。
整个过程中所有的客户端都与服务器保持连接，
由于处理时间很快，客户端感觉都能够正常与服务器通信。
本质上同一时间服务器只会对一个客户端的请求进行处理。
类似于ＣＰＵ的时间片处理方式，是一种假并发。
select()函数是通过监控一个文件描述符集合来获取有事件发生的文件描述符
FD_ZERO(fd_set *fdset) ：将 fd_set 变量所有位清零。
FD_SET(int fd, fd_set *fdset) ：在参数 fd_set 中注册文件描述符 fd 的信息。
FD_CLR(int fd, fd_set *fdset) ：在参数 fd_set 中清除文件描述符 fd 的信息。
FD_ISSET(int fd, fd_set *fdset) ：如果参数 fd_set 中已经注册了文件描述符 fd ，则返回 “真”。
成功返回大于0的发生事件的文件描述符数量，失败返回-1
int select(int maxfd, fd_set *readset, fd_set *writeset, fd_set *exceptset, const struct tieval *timeout);
- maxfd 监视文件描述符数量
- readset 关注是否存在待读数据的 fd_set 数组指针
- writeset 关注是否存在待发送无阻塞的 fd_set 数组指针
- exceptset 将所有检测到发生关注事件的文件描述符存放到 fd_set 数组中
- timeout 设置超时时间，放置 select 函数阻塞等待
*/
#define BUFFSIZE    1024

int main(int argc, char *argv[])
{
    fd_set reads, temps;
    int result, str_len;
    char buf[BUFFSIZE];
    struct timeval timeout;
    FD_ZERO(&reads);
    FD_SET(0, &reads);


    while(1)
    {
        temps = reads;
        //设置超时时间 5.5 s
        timeout.tv_sec = 5;
        timeout.tv_usec = 5000;
        //清空缓冲区内容
        memset(buf, 0, BUFFSIZE);

        result = select(1, &temps, 0, 0, &timeout);
        if(result < 0)
        {
            std::cout << "select() error. " << std::endl;
            break;
        }
        else if(result == 0)
        {
            //超时
            std::cout << "Time-out!" << std::endl;
            continue;
        }
        //判断是否是标准输入有数据到
        if(FD_ISSET(0, &temps))
        {
            str_len = read(0, buf, BUFFSIZE);
            buf[str_len] = 0;
            std::cout << "Message from console : " << buf << std::endl;
        }
    }

    return 0;
}