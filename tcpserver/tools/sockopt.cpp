#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
/*
    套接字可选项是分层的:
    其中 IPPROTO_IP 层可选项是 IP 协议相关事项
    IPPROTO_TCP 层可选项是 TCP 协议相关事项
    SOL_SOCKET 层是套接字相关的通用可选项
    
    读取套接字可选项值，成功返回0，失败返回 -1
    int getsockopt(int sock, int level, int optname, void *optval, socklen_t *optlen);
    
    - sock 用于查看选项套接字文件描述符
    - level 要查看的可选项的协议层
    - optname 要查看的可选项名称
    - optval 获取查看结果的缓冲地址
    - optlen 保存查看结果的缓冲空间占有字节大小

    设置套接字可选项值，成功返回0，失败返回 -1
    int setsockopt(int sock, int level, int optname, const void *optval, socklen_t optlen);
    - sock 用于修改选项套接字文件描述符
    - level 要修改的可选项的协议层
    - optname 要修改的可选项名称
    - optval 保存要更改的可选项信息的缓冲地址
    - optlen 第四个参数值的字节大小
*/

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
    int tcpSock, udpSock;
    int sockType;
    socklen_t optlen;
    int state;

    optlen = sizeof(sockType);

    //创建 TCP socket
    tcpSock = socket(PF_INET, SOCK_STREAM, 0);
    //创建 UDP socket
    udpSock = socket(PF_INET, SOCK_DGRAM, 0);

    std::cout << "Tcp Socket : " << tcpSock << std::endl;
    std::cout << "Udp Socket : " << udpSock << std::endl;


    state = getsockopt(tcpSock, SOL_SOCKET, SO_TYPE, (void*)&sockType, &optlen);
    if(state < 0)
    {
        error_handle("getsockopt", "getsockopt() error.");
    }
    std::cout << "Tcp Socket type : " << sockType << std::endl;

    state = getsockopt(udpSock, SOL_SOCKET, SO_TYPE, (void*)&sockType, &optlen);
    if(state < 0)
    {
        error_handle("getsockopt", "getsockopt() error.");
    }
    std::cout << "Udp Socket type : " << sockType << std::endl;

    return 0;
}