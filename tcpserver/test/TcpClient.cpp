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
    int sock, ret;
    char message[BUFFSIZE];
    int str_len;
    struct sockaddr_in serv_adr;

    if(argc < 3)
    {
        std::cout << "Usage : " << argv[0] << " <IP> <port>" << std::endl;
        exit(1);
    }

    sock = socket(PF_INET, SOCK_STREAM, 0);
    if(sock < 0)
    {
        error_handle("socket", "socket() error.");
    }

    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_adr.sin_port = htons(atoi(argv[2]));

    ret = connect(sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr));
    if(ret < 0)
    {
        error_handle("connect", "connect() error.");
    }

    std::cout << "Connect Success..." << std::endl;

    //进入数据处理
    while(1)
    {
        std::cout << "Please Input Message(Q to quit) : " << std::endl;
        std::cin >> message;
        if((!strcmp(message, "q")) || (!strcmp(message, "Q")))
        {
            //退出客户端
            break;
        }

        //将数据发送给服务端
        write(sock, message, strlen(message));
        //读取服务端回传的数据
        str_len = read(sock, message, BUFFSIZE-1);
        message[str_len] = 0;
        //打印输出
        std::cout << "Echo Message : " << message << std::endl;
    }

    //关闭写通道
    shutdown(sock, SHUT_WR);
    std::cout << "客户端进入半关闭状态，不再发送数据" << std::endl;
    //关闭套接字
    close(sock);
    std::cout << "完全关闭 TCP 连接" << std::endl;
    return 0;
}