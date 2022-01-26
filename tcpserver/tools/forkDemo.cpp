#include <iostream>
#include <unistd.h>

int main(int argc, char *argv[])
{
    pid_t pid;

    pid = fork();
    if(pid < 0)
    {
        std::cout << " fork() error." << std::endl;
        exit(1);
    }
    else if(pid == 0)
    {
        //进入子进程
        std::cout << "Init child process : pid = " << getpid() << std::endl;
        while(1)
        {
            //循环等待
            sleep(2);
        }
    }
    //父进程路径
    std::cout << "Parent process : pid = " << getpid() << std::endl;
    while(1)
    {
        //循环等待
        sleep(2);
    }

    return 0;
}