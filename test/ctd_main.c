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
#include "fifo.h"
struct tty_msg{
        unsigned char type;
        char data[0];
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
int get_mpu6050_data(unsigned char data[], int fd)
{
	int i;
	struct mpu6050_data pdata;
	struct tty_msg* msg = malloc(sizeof(struct tty_msg) + sizeof(struct mpu6050_data));
	msg->type = 0xE0;
	
	for(i=0;i<33;i++){
		if(data[i] == 0x55 && data[i+1] == 0x51){
			memset(&pdata, 0, sizeof(struct mpu6050_data));
			pdata.a[0] = (data[i+3]<<8| data[i+2])/32768.0*16;
			pdata.a[1] = (data[i+5]<<8| data[i+4])/32768.0*16;
			pdata.a[2] = (data[i+7]<<8| data[i+6])/32768.0*16;
			printf("a = %4.3f\t%4.3f\t%4.3f\t\r\n",pdata.a[0],pdata.a[1],pdata.a[2]);
			pdata.w[0] = (data[i+14]<<8| data[i+13])/32768.0*2000;
                        pdata.w[1] = (data[i+16]<<8| data[i+15])/32768.0*2000;
                        pdata.w[2] = (data[i+18]<<8| data[i+17])/32768.0*2000;
                        printf("w = %4.3f\t%4.3f\t%4.3f\t\r\n",pdata.w[0],pdata.w[1],pdata.w[2]);
			pdata.angle[0] = (data[i+25]<<8| data[i+24])/32768.0*180;
                        pdata.angle[1] = (data[i+27]<<8| data[i+26])/32768.0*180;
                        pdata.angle[2] = (data[i+29]<<8| data[i+28])/32768.0*180;
                        printf("angle = %4.3f\t%4.3f\t%4.3f\t\r\n",pdata.angle[0],pdata.angle[1],pdata.angle[2]);
			memcpy(msg->data, &pdata, (size_t) sizeof(struct mpu6050_data));	
			fifo_tx(fd, msg, sizeof(struct tty_msg) + sizeof(struct mpu6050_data));
		}
	}
	free(msg);
	return 0;
}

int main(int argc, char **argv)
{
	fd_set recv_fds;
	unsigned char buff[33];
	int fd_result;
	int maxfd = 0;
	struct timeval tv;
	tty_info *gyr_tty = uart_init(0, 9600);
		if(gyr_tty == NULL)
			return -1;
	
	tv.tv_sec = 50;    //10ms
	tv.tv_usec = 0;
	if(gyr_tty->fd > maxfd)
		maxfd = gyr_tty->fd;
	int gyr_fd = fifo_open("./gyr_fifo");
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
				get_mpu6050_data(buff, gyr_fd);
			}
		}
	}
	fifo_close(gyr_fd);
	clean_tty(gyr_tty);
	return 0;
}
