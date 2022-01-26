#include <iostream>
#include <unistd.h>
#include <sys/socket.h>



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
    int sock;
    int sendBuf = 1024, recvBuf = 4096;
    int state;

    socklen_t optlen;

    //创建socket
    sock = socket(PF_INET, SOCK_STREAM, 0);

    //设置socket 可选项 SO_SNDBUF SO_RCVBUF
    state = setsockopt(sock, SOL_SOCKET, SO_SNDBUF, (void*)&sendBuf, sizeof(sendBuf));
    if(state < 0)
    {
        error_handle("setsockopt", "setsockopt() error.");
    }
    state = setsockopt(sock, SOL_SOCKET, SO_RCVBUF, (void*)&recvBuf, sizeof(recvBuf));
    if(state < 0)
    {
        error_handle("setsockopt", "setsockopt() error.");
    }

    std::cout << "setsockopt end..." <<std::endl;

    //获取socket 可选项 SO_SNDBUF SO_RCVBUF
    optlen = sizeof(sendBuf);
    state = getsockopt(sock, SOL_SOCKET, SO_SNDBUF, (void*)&sendBuf, &optlen);
    if(state < 0)
    {
        error_handle("getsockopt", "getsockopt() error.");
    }
    //打印输出
    std::cout << "Send Buf size : " << sendBuf << std::endl;

    optlen = sizeof(recvBuf);
    state = getsockopt(sock, SOL_SOCKET, SO_RCVBUF, (void*)&recvBuf, &optlen);
    if(state < 0)
    {
        error_handle("getsockopt", "getsockopt() error.");
    }
    //打印输出
    std::cout << "Recv Buf size : " << recvBuf << std::endl;

    return 0;
}