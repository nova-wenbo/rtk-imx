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
#include "debuglog.h"
#include "fifo.h"
#include "common.h"

int main(int argc, char **argv)
{
	fd_set recv_fds;
        int fd_result;
        int maxfd = 0;
        struct timeval tv;
	struct mpu6050_data gyr;
	struct tty_msg *gyr_data = malloc(sizeof(struct tty_msg) + sizeof(struct mpu6050_data));
	if(gyr_data == NULL){
		sys_log("malloc mem failed");
		return -1;
	}
	int gyr_fd = fifo_open("./gyr_fifo");

	tv.tv_sec = 50;    //50ms
        tv.tv_usec = 0;
        if(gyr_fd > maxfd)
                maxfd = gyr_fd;
	for(;;){

                FD_ZERO(&recv_fds);
                FD_SET(gyr_fd, &recv_fds);
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
                        if(FD_ISSET(gyr_fd, &recv_fds)){
        			fifo_rx(gyr_fd, gyr_data, sizeof(struct tty_msg) + sizeof(struct mpu6050_data)); 
				printf("type : %02x \n",gyr_data->type);
				memcpy(&gyr, gyr_data->data, sizeof(struct mpu6050_data));	
				printf("a = %4.3f\t%4.3f\t%4.3f\t\r\n",gyr.a[0],gyr.a[1],gyr.a[2]);
				printf("w = %4.3f\t%4.3f\t%4.3f\t\r\n",gyr.w[0],gyr.w[1],gyr.w[2]);
				printf("angle = %4.3f\t%4.3f\t%4.3f\t\r\n",gyr.angle[0],gyr.angle[1],gyr.angle[2]);
				                       
                                
                        }
                }
        }
	fifo_close(gyr_fd);
        free(gyr_data);
	return 0;
}
