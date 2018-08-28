#ifndef __SEMAPHORE_H__
#define __SEMAPHORE_H__
#include <semaphore.h>

#define  SEM_ERR   -1
#define  SEM_WAIT_TIMEOUT   -2

typedef sem_t sys_sem_t;

int wait();
int wait(int minisecondsTimeout);
int post();
int getValue(int* value);
sys_sem_t m_sem_t;
#endif // __SEMAPHORE_H__
