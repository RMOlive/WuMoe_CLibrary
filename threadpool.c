#include <stdlib.h>
#include <pthread.h>
#include "threadpool.h"

typedef struct Thread_Task {
    void (*function)(void *);
    void *argument;
    struct Thread_Task *next;
} Thread_Task;

typedef struct Thread_Pool {
    Thread_Task *head;
    size_t shutdown;
    pthread_mutex_t lock;
    pthread_cond_t notify;
    pthread_t *threads;
    size_t maxnum_thread;
} Thread_Pool;

static void *thread_pool_thread(void *thread_pool);

Thread_Pool *new_thread_pool(size_t maxnum_thread) {
    Thread_Pool *new_pool = (Thread_Pool *) malloc(sizeof(Thread_Pool));
    new_pool->maxnum_thread = maxnum_thread;
    new_pool->shutdown = 0;
    new_pool->head = NULL;
    new_pool->threads = (pthread_t *)malloc(sizeof(pthread_t) * new_pool->maxnum_thread);
    pthread_mutex_init(&new_pool->lock, NULL);
    pthread_cond_init(&new_pool->notify, NULL);
    for(int i = 0; i < maxnum_thread; ++i)
        pthread_create(&new_pool->threads[i],NULL,thread_pool_thread,new_pool);
    return new_pool;
}

void thread_pool_add(Thread_Pool *pool, void (*routine)(void *), void *argument) {
    Thread_Task *task = (Thread_Task *) malloc(sizeof(Thread_Task));
    task->function = routine;
    task->argument = argument;
    task->next = NULL;
    pthread_mutex_lock(&pool->lock);
    Thread_Task *backups = pool->head;
    if(!backups)
        pool->head = task;
    else {
        while(backups->next)
            backups = backups->next;
        backups->next = task;
    }
    pthread_mutex_unlock(&pool->lock);
    pthread_cond_signal(&pool->notify);
}

void thread_pool_destroy(Thread_Pool *pool) {
    pool->shutdown = 1;
    pthread_mutex_lock(&pool->lock);
    pthread_cond_broadcast(&pool->notify);
    pthread_mutex_unlock(&pool->lock);
    for(int i = 0; i < pool->maxnum_thread; ++i)
        pthread_join(pool->threads[i],NULL);
    free(pool->threads);
    Thread_Task *backups;
    while(pool->head) {
        backups = pool->head;
        pool->head = pool->head->next;
        free(backups);
    }
    pthread_mutex_destroy(&pool->lock);
    pthread_cond_destroy(&pool->notify);
    free(pool);
}

static void *thread_pool_thread(void *thread_pool) {
    Thread_Pool *pool = (Thread_Pool *) thread_pool;
    Thread_Task *task;
    while (1) {
        pthread_mutex_lock(&pool->lock);
        while(!pool->head && !pool->shutdown)
            pthread_cond_wait(&pool->notify,&pool->lock);
        if(pool->shutdown) {
            pthread_mutex_unlock(&pool->lock);
            pthread_exit(NULL);
        }
        task = pool->head;
        pool->head = pool->head->next;
        pthread_mutex_unlock(&pool->lock);
        task->function(task->argument);
        free(task);
    }
}
