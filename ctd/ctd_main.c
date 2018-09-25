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
#include <signal.h>
#include "serialport.h"
#include "debuglog.h"
#include "common.h"
#include "fifo.h"
#include "gps.h"
tty_info *uart_init(int id, int speed)
{
	tty_info *putty = ready_tty(id);
	if(putty == NULL){
                sys_log("ready uart%d faild", id);
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
			fifo_tx(fd, &pdata, sizeof(struct mpu6050_data));
		}
	}
	return 0;
}
void *tmp_uart_alarm(void *arg)
{
	unsigned char tmp_send[] = {0x54, 0x50, 0x01, 0xF1, 0x96};
	unsigned char tmp_recv[7] = {0};
	//int i = 0;
	unsigned char data = 0;
	sys_log("tmp");
	tty_info *tmp_tty = uart_init(0, 9600);
	if(tmp_tty == NULL)
        	exit(-1);
	int tmp_fd = fifo_open("./tmp_fifo");
        if(tmp_fd < 0){
        	sys_log("open or create fifo faild");
                exit(-1);
        }
	for(;;){
		sleep(3);
		int count = sendn_tty(tmp_tty, tmp_send, sizeof(tmp_send));
		if(count < 0)
			exit(-1);
		usleep(10000);
		count = recvn_tty(tmp_tty, tmp_recv, sizeof(tmp_recv));
		sys_log("tmp read %d", count);
		//for(i=0;i<count;i++)
		//	printf("0x%02x ", tmp_recv[i]);
		data = (tmp_recv[4]*256 + tmp_recv[5])/10;
		fifo_tx(tmp_fd, &data, sizeof(uchar));
		printf("tmp : %d\n", data);
	}
	close(tmp_fd);
	clean_tty(tmp_tty);	
}
int main(int argc, char **argv)
{
	fd_set recv_fds;
	unsigned char buff[33];
	int fd_result;
	int maxfd = 0;
	struct timeval tv;
	int gyr_fd = -1;
	pthread_t   tid;
	tty_info *gyr_tty = uart_init(2, 9600);
	if(gyr_tty == NULL){
		sys_log("uart init faild");
	}
	gyr_fd = fifo_open("./gyr_fifo");
	if(gyr_fd < 0){
               sys_log("open or create fifo faild");
                return -1;
       	}
	
	int err = pthread_create(&tid, NULL, tmp_uart_alarm, NULL);
	if(err != 0)  
        	sys_log("can't create thread 1: %d\n", strerror(err));
	if(gyr_tty->fd > maxfd)
                maxfd = gyr_tty->fd;
	tv.tv_sec = 360;    //1 hour
	tv.tv_usec = 0;

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
		else{
			if(FD_ISSET(gyr_tty->fd, &recv_fds)){
				recvn_tty(gyr_tty,buff,sizeof(buff));
				get_mpu6050_data(buff, gyr_fd);
			}
		}
	}
	pthread_exit(&tid);
	fifo_close(gyr_fd);
	clean_tty(gyr_tty);
	return 0;
}
