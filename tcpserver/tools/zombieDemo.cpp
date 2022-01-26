#include <iostream>
#include <unistd.h>

int main(int argc, char *argv[])
{
    pid_t pid;

    pid = fork();
    if(pid < 0)
    {
        std::cout << "fork() error." << std::endl;
        exit(1);
    }
    else if(pid == 0)
    {
        //进入子进程
        std::cout << "Child process , pid = " << getpid() << std::endl;
    }
    else{
        //父进程进入休眠
        std::cout << "Parent process, pid = " << getpid() << std::endl;
        sleep(50);
    }

    if(pid == 0)
    {
        //子进程打印结束输出
        std::cout << "Child process end..." << std::endl;
    }
    else{
        //父进程打印结束输出
        std::cout << "Parent process end..." << std::endl;
    }

    return 0;
}