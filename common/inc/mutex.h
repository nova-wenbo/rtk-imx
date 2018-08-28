#ifndef __MUTEX_H__
#define __MUTEX_H__

typedef pthread_muxtex_t sys_mutex_t;

void lock();
bool try_lock();
void unlock();
sys_mutex_t  m_mutex_t;

#endif
