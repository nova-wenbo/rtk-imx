#ifndef __SEM_H__
#define __SEM_H__
int sem_init(sem_t *m_sem_t, int pshared, unsigned int value);
int wait(sem_t *m_sem_t);
int wait(sem_t *m_sem_t, int minisecondsTimeout);
int post(sem_t *m_sem_t);
int getvalue(sem_t *m_sem_t);
#endif
