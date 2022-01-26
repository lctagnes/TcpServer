#include <iostream>
#include <unistd.h>
#include <cstring>
#include <sys/time.h>
#include <sys/epoll.h>
/*
select在运行时速度会很慢的，主要原因是：
1.调用 select 函数后必然会执行针对所有可用文件描述符的循环语句；
2.每次调用 select 函数时都需要向该函数传递监视对象信息，需要反复初始化参数进行传递；
epoll 之所以要优于 select 方式，主要是有以下优点：
1.无需编写用来监控状态变化为目的的针对所有文件描述符的循环语句；
2.调用对应于 select 函数的 epoll_wait 函数不需要反复传递监控对象的信息；

创建保存epoll文件描述符的空间，类似创建select方式下的fd_set变量
int epoll_create(int size);//size ：epoll 实例大小

注册或者注销监视的文件描述符，类似select方式下的FD_SET、FD_CLR操作
int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);
epfd：用于注册监视对象的epoll文件描述符，前面epoll_create函数的返回值
op：操作参数，传递监视对象的添加、删除、更改的操作
    EPOLL_CTL_ADD：将文件描述符注册到epoll例程中
    EPOLL_CTL_DEL：从epoll例程中删除文件描述符
    EPOLL_CTL_MOD：更改注册的文件描述符的关注事件发生情况
fd：需要操作的监视对象文件描述符
event：监视对象的事件类型。通过初始化一个 epoll_event 结构体的变量完成注册事件的传递，其中event.events成员变量由下面内容组成：
    EPOLLIN：需要读取数据的情况
    EPOLLOUT：输出缓冲为空，可以立即发送数据的情况
    EPOLLPRI：收到 OOB 数据的情况
    EPOLLRDHUP：断开连接或者半关闭的情况，在边缘触发方式下经常使用
    EPOLLERR：发送错误的情况
    EPOLLET：以边缘触发的方式得到事件通知
    EPOLLONESHOT：发送一次事件后，对应的文件描述符不再收到事件通知，如果还需要获取事件通知，需要使用 epoll_ctl 再次设置事件

不同于select方式去轮询探查监视对象，epoll是等待事件触发
int epoll_wait(int epfd, struct epoll_event *event, int maxevents, int timeout);
epfd：epoll例程的文件描述符
events：保存发生事件的文件描述符集合的结构体地址，该参数所指向的缓冲需要动态分配
maxevents：第二个参数中科院保存的最大事件数量
timeout：以微秒为单位的等待时间，设置为-1则表示持续等待
*/
#define BUFFSIZE    1024

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