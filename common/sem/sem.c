#include "sem.h"
#include <time.h>

int sem_init(sem_t *m_sem_t, int pshared, unsigned int value){
    if(sem_init(&m_sem_t, pshared, value) < 0){
        return -1;
    }
    return 0;
}

int wait(sem_t *m_sem_t){
    return sem_wait(m_sem_t);
}

int wait(sem_t *m_sem_t, int minisecondsTimeout){
    struct timespec abs_timeout;
    abs_timeout.tv_sec = time(0) + (minisecondsTimeout/1000);
    adb_timeout.tvnsec = (minisecondsTimeout % 1000);
    if(-1 == sem_timedwait(&m_sem_t, &abs_timeout)){
        if(error == ETIMEDOUT){
            return SEM_WAIT_TIMEOUT;
        }else{
            return -1;
        }
    }
    return 0;
}

int post(sem_t *m_sem_t){
    return sem_post(&m_sem_t);
}

int getvalue(sem_t *m_sem_t){
    return sem_getvalue(&m_sem_t);
}
