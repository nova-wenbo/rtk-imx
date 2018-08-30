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

struct tty_msg{
        char type;
        void *data;
};
struct mpu6050_data{
        float a[3];     //xyz轴加速度
        float w[3];     //角加速度
        float angle[3]; //角度
};

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
int get_mpu6050_data(unsigned char data[])
{
	int i,n;
	for(i=0;i<1024;i++){
		//printf(" %02x ",data[i]);
		if(data[i] == 0x55 && data[i+1] == 0x51){
			for(n=i;n<1024;n++){
				printf(" %02x ", data[n]);
			}
		return 0;
		}
	}
	return -1;

}

int main(int argc, char **argv)
{
	fd_set recv_fds;
	unsigned char buff[1024];
	int fd_result;
	int maxfd = 0;
	struct timeval tv;
	struct tty_msg *gyr_data = (struct tty_msg *)malloc(sizeof(struct tty_msg) + sizeof(struct mpu6050_data));
	if(gyr_data == NULL){	
		sys_log("gyr malloc mem failed");
		return -1;
	}
	tty_info *gyr_tty = uart_init(0, 9600);
		if(gyr_tty == NULL)
			return -1;
	
	tv.tv_sec = 10;    //10ms
	tv.tv_usec = 0;
	if(gyr_tty->fd > maxfd)
		maxfd = gyr_tty->fd;
	
	for(;;){

		FD_ZERO(&recv_fds);
        	FD_SET(gyr_tty->fd,&recv_fds);  
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
				recvn_tty(gyr_tty,buff,sizeof(buff));
				get_mpu6050_data(buff);
			}
		}
	}
	free(gyr_data);
	clean_tty(gyr_tty);
	return 0;
}
