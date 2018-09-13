#include<stdio.h>
#include<fcntl.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>


int fifo_open(const char *fifo_name){
    int fifo_fd;
    if(access(fifo_name, F_OK) == -1){
        mkfifo(fifo_name, 0777);
    }
    fifo_fd = open(fifo_name, O_RDWR);
    if(fifo_fd < 0 ){
        return -1;
    }
    return fifo_fd;
}

int fifo_tx(int fifo_fd, void *data, int len){

    int nbytes = -1;
    while( len > 0 ){
        nbytes = write(fifo_fd, data, len);
        if(nbytes < 0){
            return -1;
        }
        len -= nbytes;
        data += nbytes;
    }
    return len;
}

int fifo_rx(int fifo_fd, void *data, int len){
    int nbytes = -1;
    nbytes = read(fifo_fd, data, len);
    return nbytes;
}

void fifo_close(int fifo_fd){
    close(fifo_fd);
    return;
}
