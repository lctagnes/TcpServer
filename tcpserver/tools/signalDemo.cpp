#include <iostream>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

//出错调用函数
static void error_handle(std::string opt, std::string message)
{
    //根据errno值获取失败原因并打印到终端
    perror(opt.c_str());
    std::cout << message << std::endl;
    exit(1);
}


void child_process_handle(int sig)
{
    int re;
    int pid;
    //非阻塞循环  因为信号不排队所以可能有几个子进程退出信号叠加在一起所以要循环
    while((pid = waitpid(-1,&re, WNOHANG)) > 0)
    {
        if(WIFEXITED(re))
        {
            std::cout << pid << " exit " << WEXITSTATUS(re) << std::endl;
        }
        if(WIFSIGNALED(re))
        {
            std::cout << pid << " exit " << WTERMSIG(re) << std::endl;
        }
    }
}

int main(int argc, char *argv[])
{
    int pid;
    int i = 10;
    while(i--)
    {
        if((pid = fork()) < 0)  //错误
        {
            error_handle("fork", "fork() error.");
        }
        else if(pid == 0)   //子进程
        {
            break;
        }
        else if(pid > 0)
        {
            continue;
        }
    }
    if(pid > 0)
    {
        struct sigaction act;
        act.sa_handler = child_process_handle;//设置信号处理函数
        sigemptyset(&act.sa_mask);
        act.sa_flags = 0;
        if(sigaction(SIGCHLD, &act, NULL) < 0)
        {
            error_handle("sigaction", "sigaction() error.");
        }
        while(1)
        {
            //父进程循环等待处理回收子进程
        }

    }
    if(pid == 0)
    {
        sleep(10);
    }
    return 0;
}