#include <iostream>
#include <pthread.h>
/*
当多个线程同时访问同一个全局变量时就让容易导致结果不可预测。
线程 1 虽然完成了计算，但是在还没有将结果放回到内存中时，
线程 2 就又执行了操作。这种操作由于每次发生的次序不一致
也就导致最终结果每次都不一样的情况。
*/
#define NUM_THREAD 100

//自增线程
void *thread_inc(void *arg);
//自减线程
void *thread_des(void *arg);
//全局变量
long long num = 0;

int main(int argc, char *argv[])
{
    pthread_t thread_id[NUM_THREAD];

    //创建50个加线程，创建50个减线程
    for(int i = 0; i < NUM_THREAD; ++i)
    {
        if(i%2)
            pthread_create(&(thread_id[i]), NULL, thread_inc, NULL);
        else
            pthread_create(&(thread_id[i]), NULL, thread_des, NULL);
    }

    //回收这100个线程资源
    for(int i = 0; i< NUM_THREAD; ++i)
    {
        pthread_join(thread_id[i], NULL);
    }

    //打印输出结果
    std::cout << "Result : " << num << std::endl;
    return 0;
}

//自增线程
void *thread_inc(void *arg)
{
    for(int i = 0; i < 3000; ++i)
    {
        num++;
    }
    return NULL;
}
//自减线程
void *thread_des(void *arg)
{
    for(int i = 0; i < 3000; ++i)
    {
        num--;
    }
    return NULL;
}