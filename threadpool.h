#ifndef __WUMOE_THREADPOOL_H
#define __WUMOE_THREADPOOL_H

typedef struct Thread_Pool Thread_Pool;

Thread_Pool *new_thread_pool(size_t maxnum_thread);

void thread_pool_add(Thread_Pool *pool, void (*routine)(void *), void *argument);

void thread_pool_destroy(Thread_Pool *pool);

#endif
