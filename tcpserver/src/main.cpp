#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <cstring>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "base.h"

//出错调用函数
static void error_handle(std::string opt, std::string message)
{
    //根据errno值获取失败原因并打印到终端
    perror(opt.c_str());
    std::cout << message << std::endl;
    exit(1);
}

//子进程处理客户端请求函数封装
int ChildProcessWork(int ServSocket, int ChildSocket)
{
    char buf[BUFFSIZE] = {0};
    int str_len;
    //首先在子进程中关闭服务端套接字
    close(ServSocket);

    while(1)
    {
        //接收客户端数据
        str_len = read(ChildSocket, buf, BUFFSIZE);
        if(str_len == 0)
        {
            //客户端断开连接，处理结束
            break;
        }
        //多进程服务中不再将受到数据打印输出
        //将客户端数据返回给客户端
        write(ChildSocket, buf, str_len);
        memset(buf, 0, BUFFSIZE);
    }

    //关闭客户端套接字
    close(ChildSocket);
    return 0;
}

//处理子进程结束后，父进程回收资源的信号回调函数
void read_childproc(int signum)
{
    pid_t pid;
    int status;
    pid = waitpid(-1, &status, WNOHANG);
    std::cout << "Removed proc id : " << pid << std::endl;
}

//多进程工作
int fork_workproc(int serv_sock)
{
    struct sockaddr_in client_adr;
    socklen_t adr_size;
    int client_sock, ret;
    pid_t pid;

    //循环等待客户端请求
    while (1)
    {
        adr_size = sizeof(client_adr);
        client_sock = accept(serv_sock, (struct sockaddr*)&client_adr, &adr_size);
        if(client_sock < 0)
        {
            //接收请求失败，继续工作
            continue;
        }
        else{
            //接收到新的客户端，打印输出
            std::cout << "New Client IP : " << inet_ntoa(client_adr.sin_addr) << " , port : " << ntohs(client_adr.sin_port) << std::endl;
            //创建子进程进行处理
            pid = fork();
            if(pid < 0)
            {
                //创建子进程失败，关闭连接
                std::cout << "fork error, close client" << std::endl;
                close(client_sock);
                continue;
            }
            if(pid == 0)
            {
                //进入子进程,
                ret = ChildProcessWork(serv_sock, client_sock);
                //调用结束后直接结束子进程
                exit(ret);
            }
            else{
                //父进程关闭客户端套接字，继续等待新客户端请求
                close(client_sock);
            }
        }
    }
    return 0;
}

//select I/O复用
int select_workproc(int serv_sock)
{
    struct sockaddr_in client_adr;
    socklen_t adr_size;
    int client_sock, ret;
    int fd_max, fd_num;
    fd_set reads, cpy_reads;
    struct timeval timeout;
    char buf[BUFFSIZE];
    int str_len;

    FD_ZERO(&reads);
    FD_SET(serv_sock, &reads);
    fd_max = serv_sock;

    while(1)
    {
        cpy_reads = reads;
        timeout.tv_sec = 5;
        timeout.tv_usec = 5000;

        fd_num = select(fd_max + 1, &cpy_reads, 0, 0, &timeout);
        if(fd_num < 0)
        {
            //调用select 失败
            std::cout << "select error." << std::endl;
            break;
        }
        else if(fd_num == 0)
        {
            //目前没有数据事件发生
            continue;
        }
        //循环处理当前所有可读事件
        for(int i = 0; i < fd_max + 1; i++)
        {
            if(FD_ISSET(i, &cpy_reads))
            {
                //如果当前要处理的文件描述符是 serv_sock 服务器套接字，则表示需要处理客户端请求连接事件
                if(i == serv_sock)
                {
                    adr_size = sizeof(client_adr);
                    client_sock = accept(serv_sock, (struct sockaddr*)&client_adr, &adr_size);
                    //打印新连接客户端信息
                    std::cout << "New Client IP : " << inet_ntoa(client_adr.sin_addr) << " , port : " << ntohs(client_adr.sin_port) << std::endl;
                    //将新连接的客户端注册到select处理队列中
                    FD_SET(client_sock, &reads);
                    //如果当前最大值小于新客户端套接字，则改变
                    if(fd_max < client_sock)
                    {
                        fd_max = client_sock;
                    }
                }
                else
                {
                    //处理客户端发来的消息，echo服务器负责将数据返回
                    str_len = read(i, buf, BUFFSIZE);
                    if(str_len == 0)
                    {
                        //客户端连接断开
                        //将客户端套接字从select监控队列中清除
                        FD_CLR(i, &reads);
                        close(i);
                        //输出打印
                        std::cout << "Closed Client : " << i << std::endl;
                    }
                    else
                    {
                        //正常处理数据，将读取到的数据返回给客户端
                        write(i, buf, str_len);
                    }

                }
            }
        }
    }

    return 0;
}


int main(int argc, char *argv[])
{
    int serv_sock, ret;
    struct sockaddr_in serv_adr;
    struct sigaction act;

    int str_len, ret;
    int option;
    socklen_t optlen;
    char buf[BUFFSIZE];

    //判断命令行参数合法性
    if(argc < 2)
    {
        std::cout << "Usage : " << argv[0] << " <port>" << std::endl;
        exit(1);
    }

    act.sa_handler = read_childproc;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    state = sigaction(SIGCHLD, &act, 0);

    //创建socket套接字
    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    if(serv_sock < 0)
    {
        error_handle("socket", "socket() error.");
    }

    //添加 Time-wait 状态重新分配断开设置
    /* 
    如果是客户端先断开连接，不存在什么问题。
    如果是服务器先调用close断开连接，短时间内重启服务器会出现bind error的问题。
    默认等待 2 到 3 分钟再次启动服务器就正常了。只需要修改套接字可选项 SO_REUSEADDR 即可。
    套接字可选项 SO_REUSEADDR 默认值为 0 ，表示不会重新分配 Time-wait 状态下的套接字端口，
    也就是在 TCP 连接进入 Time-wait 状态后依然占用之前的 TCP 端口号。
    如果将 SO_REUSEADDR 设置为 1 ，那么当 TCP 连接进入 Time-wait 状态后就会重新分配端口号，
    而不会占用之前端口，这样就不会影响新的服务再次启动了。
    */
    optlen = sizeof(option);
    option = 1;
    setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, (void*)&option, optlen);

    //初始化服务器套接字参数，设置网卡IP 和端口号
    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(atoi(argv[1]));

    //绑定端口
    ret = bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr));
    if(ret < 0)
    {
        error_handle("bind", "bind() error.");
    }

    //监听TCP端口号
    ret = listen(serv_sock, 5);
    if(ret < 0)
    {
        error_handle("listen", "listen() error.");
    }

    // //多进程方式实现
    // fork_workproc(serv_sock);
    //使用select I/O复用方式
    select_workproc(serv_sock);
    //服务器关闭，关闭服务器套接字
    close(serv_sock);
    return 0;
}