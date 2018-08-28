#ifndef __SHM_H__
#define __SHM_H__
#include<linux/types.h>
extern int shm_init(void *);
extern void *shm_alloc(key_t key, size_t size, int shmflag);
extern void *shm_attach(void *);
extern int shm_detach(void *);
extern int shm_free(int shmid);
#endif
~        
