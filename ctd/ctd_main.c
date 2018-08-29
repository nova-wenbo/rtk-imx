#include <stdio.h>  
#include <stdlib.h>   
#include <unistd.h>    
#include <fcntl.h>   
#include <termios.h>  
#include <errno.h>  
#include <string.h>  
#include <pthread.h>
#include <sys/types.h>  
#include <sys/stat.h>
#include "serialport.h"
#include "debuglog.h"
#include "common.h"

tty_info *uart_init(int id, int speed)
{
	tty_info *putty = ready_tty(id);
	if(putty == NULL){
                sys_log("ready gyr uart%d faild", id);
		return NULL;
	}
	lock_tty(putty);
	if(set_ttySpeed(putty, speed)>0){
                sys_log("set uart Speed() error");
		return NULL;
	}
        if(set_ttyParity(putty,8,'N',1)>0){
                sys_log("set uart Parity() error");
		return NULL;
	}
	return putty;
}


int main(int argc, char **argv)
{
	fd_set recv_fds;
	unsigned char buff[64];
	unsigned int nbyte = 0;
	int fd_result;
	int maxfd = 0;
	struct timeval tv;
	struct tty_msg *gyr_data = (struct tty_msg *)malloc(sizeof(struct tty_msg));
	if(gyr_data == NULL){	
		sys_log("gyr malloc mem failed");
		return -1;
	}
	struct tty_msg *tmp_data = (struct tty_msg *)malloc(sizeof(struct tty_msg));
	if(tmp_data == NULL){
                sys_log("tmp malloc mem failed");
		return -1;
	}
	struct tty_msg *rtk_data = (struct tty_msg *)malloc(sizeof(struct tty_msg));
	if(rtk_data == NULL){
		sys_log("rtk malloc mem failed");
		return -1;
	}
	tty_info *gyr_tty = uart_init(0, 115200);
		if(gyr_tty == NULL)
			return -1;
	
	tty_info *tmp_tty = uart_init(1, 115200);
		if(tmp_tty == NULL)
			return -1;
	
	tty_info *rtk_tty = uart_init(2, 115200);
		if(rtk_tty == NULL)
			return -1;
	
	tv.tv_sec = 10;    //10ms
	tv.tv_usec = 0;
	if(gyr_tty->fd > maxfd)
		maxfd = gyr_tty->fd;
	if(tmp_tty->fd > maxfd)
		maxfd = tmp_tty->fd;
	if(rtk_tty->fd > maxfd)
		maxfd = rtk_tty->fd;
	
	sys_log("test test test ");
	for(;;){

		FD_ZERO(&recv_fds);
        	FD_SET(gyr_tty->fd,&recv_fds);  
        	FD_SET(tmp_tty->fd,&recv_fds);
		FD_SET(rtk_tty->fd,&recv_fds);
		fd_result = select(maxfd + 1, &recv_fds, NULL, NULL, &tv);
		if(fd_result < 0)
        	{
                	sys_log("select err");
            		usleep(10000);
            		continue;
        	}
		else if(fd_result == 0){
			sys_log("select wait data");
			usleep(10000);
                        continue;
		}
		else {
			if(FD_ISSET(gyr_tty->fd, &recv_fds)){
				nbyte = recvn_tty(gyr_tty,buff,64);
			}
			if(FD_ISSET(tmp_tty->fd, &recv_fds)){
                                nbyte = recvn_tty(tmp_tty,buff,64);
                        }
			if(FD_ISSET(rtk_tty->fd, &recv_fds)){
                                nbyte = recvn_tty(rtk_tty,buff,64);
                        }			
		}
	}
	free(gyr_data);
	free(tmp_data);
	free(rtk_data);
	clean_tty(gyr_tty);
	clean_tty(tmp_tty);
	clean_tty(rtk_tty);
	return 0;
}
