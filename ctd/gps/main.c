#include "gps.h"
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
#include "fifo.h"


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

int main()
{
	fd_set recv_fds;
	int fd_result;
	int maxfd = 0;
	char buff[512] = {0};
        struct timeval tv;	
	gps_info gps;
	tty_info *gps_tty = uart_init(0, 115200);
	if(gps_tty == NULL){
                sys_log("uart init faild");
        }
	int gps_fd = fifo_open("./gps_fifo");
        if(gps_fd < 0){
                sys_log("open or create fifo faild");
                exit(-1);
        }
			
	if(gps_tty->fd > maxfd)
                maxfd = gps_tty->fd;
        tv.tv_sec = 10;    //1 hour
        tv.tv_usec = 0;
	for(;;){
                FD_ZERO(&recv_fds);
                FD_SET(gps_tty->fd,&recv_fds);
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
                        if(FD_ISSET(gps_tty->fd, &recv_fds)){
				memset(buff, 0, sizeof(buff));
				memset(&gps, 0, sizeof(gps));
                              	recvn_tty(gps_tty,buff,sizeof(buff));
				gps_rmc_parse(buff, &gps);
				//gps_gga_parse(buff, &gps);
				if((int)(gps.latitude) != 0)
					fifo_tx(gps_fd, &gps, sizeof(gps));
				//printf("height : %02f, satellite : %d \n", gps.height, gps.satellite);
				printf("time : %d-%d-%d-%d:%d:%d \n",gps.D.year,gps.D.month,gps.D.day,gps.D.hour,gps.D.minute,gps.D.second);
				//printf("latitude : %d-%d-%d\n",gps.latitude_Degree,gps.latitude_Cent,gps.latitude_Second);
				printf("gps: %lf-%lf\n", gps.latitude, gps.longitude);
                        }
                }
        }
	close(gps_fd);
	clean_tty(gps_tty);
	return 0;
}
