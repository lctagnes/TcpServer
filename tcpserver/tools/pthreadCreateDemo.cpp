#include <iostream>
#include <unistd.h>
#include <pthread.h>
/*
线程相比进程具有以下优点：

线程的创建和上下文切换比进程的创建和上下文切换更快。
线程间交换数据时不需要类似进程的 IPC 操作。
占用系统资源少。
编译时需要添加 -pthread 编译参数
成功返回0， 失败返回其他值
int pthread_create(pthread_t *restrict thread, 
    const pthread_attr_t *restrict attr, 
    void *(* start_routine)(void *), 
    void *restrict arg);
- thread 保存新创建线程 ID 的变量地址值，线程与进程相同，需要用于区分不同线程的 ID
- attr 用于传递线程属性参数，NULL 时表示创建默认属性线程
- start_routine 传递线程入口函数指针
- arg 传递给线程入口函数的参数信息的变量地址
*/
void *thread_routine1(void *arg);
void *thread_routine2(void *arg);

int main(int argc, char *argv[])
{
    pthread_t t_id1, t_id2;
    int thread_param = 0;

    //创建线程1
    if(pthread_create(&t_id1, NULL, thread_routine1, (void*)&thread_param) != 0)
    {
        std::cout << "pthread create error" << std::endl;
        return -1;
    }
    //创建线程2
    if(pthread_create(&t_id2, NULL, thread_routine2, (void*)&thread_param) != 0)
    {
        std::cout << "pthread create error" << std::endl;
        return -1;
    }
    //等待1秒
    sleep(1);
    std::cout << "Main Start ..." << std::endl;
    //给参数赋值，使线程正式工作
    thread_param = 5;
    sleep(5);

    std::cout << "Main End ..." << std::endl;

    return 0;
}

void *thread_routine1(void *arg)
{
    int *cnt = (int *)arg;
    //当参数为0时，循环等待
    while(*cnt == 0)

    for(int i = 0; i < *cnt; ++i)
    {
        sleep(1);
        std::cout << "Thread 1 Runing..." << std::endl;
    }

    return NULL;
}

void *thread_routine2(void *arg)
{
    int *cnt = (int *)arg;
    //当参数为0时，循环等待
    while(*cnt == 0)

    for(int i = 0; i < *cnt; ++i)
    {
        sleep(1);
        std::cout << "Thread 2 Runing..." << std::endl;
    }

    return NULL;
}