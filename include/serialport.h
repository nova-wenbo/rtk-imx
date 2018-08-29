#ifndef __SERIALPORT_H__ 
#define __SERIALPORT_H__ 

//包含头文件
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <sys/types.h> 
#include <sys/stat.h> 
#include <fcntl.h> 
#include <termios.h> 
#include <errno.h> 
#include <pthread.h> 
#include <string.h>
#include <sys/ioctl.h>
// 串口设备信息结构 
typedef struct tty_info_t 
{ 
	int fd; // 串口设备ID 
	pthread_mutex_t mt; // 线程同步互斥对象 
	char name[24]; // 串口设备名称，例："/dev/ttyS0" 
	struct termios ntm; // 新的串口设备选项 
	struct termios otm; // 旧的串口设备选项 
} tty_info; 
// 串口操作函数 
tty_info *ready_tty(int id); 
int set_ttySpeed(tty_info *ptty, int speed); 
int set_ttyParity(tty_info *ptty,int databits,int parity,int stopbits); 
int clean_tty(tty_info *ptty); 
int sendn_tty(tty_info *ptty,char *pbuf,int size); 
int recvn_tty(tty_info *ptty,char *pbuf,int size); 
int lock_tty(tty_info *ptty); 
int unlock_tty(tty_info *ptty); 

#endif 
