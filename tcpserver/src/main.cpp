#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <cstring>
#include <sys/socket.h>
#include "base.h"

//出错调用函数
static void error_handle(std::string opt, std::string message)
{
    //根据errno值获取失败原因并打印到终端
    perror(opt.c_str());
    std::cout << message << std::endl;
    exit(1);
}

int main(int argc, char *argv[])
{
    int serv_sock, client_sock;
    char message[BUFFSIZE];
    int str_len, ret;

    struct sockaddr_in serv_adr, client_adr;
    socklen_t client_adr_size;

    //判断命令行参数合法性
    if(argc < 2)
    {
        std::cout << "Usage : " << argv[0] << " <port>" << std::endl;
        exit(1);
    }

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

    client_adr_size = sizeof(client_adr);

    while(1)
    {
        //使用accept接收客户端连接请求
        client_sock = accept(serv_sock, (struct sockaddr*)&client_adr, &client_adr_size);
        if(client_sock < 0)
        {
            //接收客户端连接请求失败，
            continue;
        }
        //接收新的客户端请求，进行客户端数据处理
        std::cout << "Accept New Client : " << inet_ntoa(client_adr.sin_addr) << " , port : " << ntohs(client_adr.sin_port) << std::endl;
        std::cout << "Start Recv Client Data..." << std::endl;

        //清空缓存区
        memset((void *)&message, 0, BUFFSIZE);
        while((str_len = read(client_sock, message, BUFFSIZE)) != 0)
        {
            //成功读取客户端发送来的数据消息
            //打印输出
            std::cout << "Recv Message : " << message << std::endl;
            //将消息回传给客户端，作为回声服务器，类似 echo 命令
            write(client_sock, message, str_len);
            //清空缓存区，等待再次读取数据
            memset(message, 0, BUFFSIZE);
        }

        //客户端断开连接，关闭套接字
        close(client_sock);
    }

    //服务器关闭，关闭服务器套接字
    close(serv_sock);
    return 0;
}