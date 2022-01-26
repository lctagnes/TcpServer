#include <iostream>
#include <unistd.h>
#include <sys/time.h>
#include <sys/epoll.h>
#include <cstring>
/*
epoll主要是依靠内核对事件触发后的消息返回。
条件触发表示当前监控的文件描述符满足注册条件就会发送通知消息，
只要输入缓冲区中还有数据就会在 epoll_wait 函数中返回这个文件描述符
（实际它并没有什么状态变化，只不过是上次数据处理不够完全）
由于每次数据处理都没能将待读取数据完全读取完全，
所以条件触发会持续注册事件使程序进行继续的处理
*/
#define BUFFSIZE    3

int main(int argc, char *argv[])
{

    char buf[BUFFSIZE];
    int timeout;
    int epfd, epoll_size, event_cnt;
    struct epoll_event *ep_events;
    struct epoll_event event;
    int str_len;

    //设置超时时间 5.5 s , 5500 微秒
    timeout = 5500;

    //创建epoll 例程，size = 1
    epoll_size = 2;
    epfd = epoll_create(epoll_size);
    if(epfd < 0)
    {
        //创建失败
        std::cout << "epoll_create error." << std::endl;
        return -1;
    }

    //创建用于epoll_wait事件发生的事件结构体缓冲区
    ep_events = (struct epoll_event *)malloc(sizeof(struct epoll_event)*epoll_size);

    //初始化文件描述符事件注册参数
    event.events = EPOLLIN; //监视事件为有数据输入/可读取状态
    event.data.fd = 0;  //监视文件描述符为标准输入
    //设置事件注册
    epoll_ctl(epfd, EPOLL_CTL_ADD, 0, &event);

    while(1)
    {
        //清空缓冲区内容
        memset(buf, 0, BUFFSIZE);

        event_cnt = epoll_wait(epfd, ep_events, epoll_size, timeout);
        if(event_cnt < 0)
        {
            std::cout << "epoll_wait() error!" << std::endl;
            break;
        }
        else if(event_cnt == 0)
        {
            std::cout << "epoll_wait() timeout!" << std::endl;
            continue;
        }
        else{
            //添加正常调用打印
            std::cout << "epoll_wait() sucess" << std::endl;
            //这里只注册了一个文件描述符，所以不需要循环处理，通常还是需要循环处理所有返回的事件消息
            //基础判断,发生事件的文件描述符是标准输入
            if(ep_events[0].data.fd == 0)
            {
                //读取数据并打印输出
                str_len = read(0, buf, BUFFSIZE);
                buf[str_len] = 0;
                std::cout << "Message from console : " << buf << std::endl;
            }

        }
    }
    //关闭epfd
    close(epfd);

    return 0;
}