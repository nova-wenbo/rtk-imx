#include <sys/cdefs.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "shm.h"
/*
* 
*/
int shm_init(void * msg){
    memset(msg, 0, sizeof(*msg));
    return 0;
}
/*
*
*/
int void * shm_alloc(key_t key, size_t size, int shmflag){
    int shmid;
    shmid = shmget(key, size, shmflg);
    return shmat(shmid, NULL, 0);
}

/*
*
*/
void * shm_attach(int shmid){
    return shmat(shmid, NULL, 0)
}
/*
*
*/
int shm_detach(void *data){
    if(shmdat(data) < 0){
        return -1;
    }
    return 0;
}
/*
* 
*/
int shm_free(int shmid){
    if(shmctl(shmid, IPC_RMID, NULL) < 0){
        return -1;
    }
    return 0;
}
