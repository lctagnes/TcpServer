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
    int sendBuf, recvBuf, state;
    socklen_t optlen;

    //创建socket
    sock = socket(PF_INET, SOCK_STREAM, 0);

    //获取socket可选项 SO_SNDBUF 的值
    optlen = sizeof(sendBuf);
    state = getsockopt(sock, SOL_SOCKET, SO_SNDBUF, (void*)&sendBuf, &optlen);
    if(state < 0)
    {
        error_handle("getsockopt", "getsockopt() error.");
    }
    //打印输出
    std::cout << "Send Buf size : " << sendBuf << std::endl;

    //获取socket可选项 SO_RCVBUF 的值
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