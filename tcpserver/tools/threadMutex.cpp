#include <iostream>
#include <pthread.h>
/*
互斥量（ mutex ）主要就是用来解决线程同步问题的。
//成功返回 0，失败返回其他值
int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr);
int pthread_mutex_destroy(pthread_mutex_t *mutex);
mutex 创建互斥量时传递保存互斥量的变量地址
attr  传递即将创建的互斥量属性，默认属性传递NULL
//保护临界区而需要添加的上锁和解锁操作
int pthread_mutex_lock(pthread_mutex_t *mutex);
int pthread_mutex_unlock(pthread_mutex_t *mutex);
*/
#define NUM_THREAD 100

//自增线程
void *thread_inc(void *arg);
//自减线程
void *thread_des(void *arg);
//全局变量
long long num = 0;

//声明使用的互斥量
pthread_mutex_t mutex;

int main(int argc, char *argv[])
{
    pthread_t thread_id[NUM_THREAD];

    //初始化互斥量
    pthread_mutex_init(&mutex, NULL);

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

    //释放互斥量
    pthread_mutex_destroy(&mutex);
    return 0;
}

//自增线程
void *thread_inc(void *arg)
{
    pthread_mutex_lock(&mutex);
    for(int i = 0; i < 3000; ++i)
    {
        num++;
    }
    pthread_mutex_unlock(&mutex);
    return NULL;
}
//自减线程
void *thread_des(void *arg)
{
    pthread_mutex_lock(&mutex);
    for(int i = 0; i < 3000; ++i)
    {
        num--;
    }
    pthread_mutex_unlock(&mutex);
    return NULL;
}