#ifndef __FIFO_H__
#define __FIFO_H__
//包含头文件
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <sys/ioctl.h>
extern int fifo_open(const char *fifo_name);
extern int fifo_tx(int fifo_fd, void *data, int len);
extern int fifo_rx(int fifo_fd, void*data, int len);
extern int fifo_close(int fifo_fd);
#endif
