#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "debuglog.h"
#include "fifo.h"
#include "common.h"


#define HELLO_WORLD_SERVER_PORT 6666 
#define LENGTH_OF_LISTEN_QUEUE 20
int main(int argc, char **argv)
{
	fd_set recv_fds;
        int fd_result;
        int maxfd = 0;
        struct timeval tv;
	struct mpu6050_data gyr;
	struct msg gyr_msg;
	struct sockaddr_in server_addr;
    	bzero(&server_addr,sizeof(server_addr)); //把一段内存区的内容全部设置为0
    	server_addr.sin_family = AF_INET;
    	server_addr.sin_addr.s_addr = htons(INADDR_ANY);
    	server_addr.sin_port = htons(HELLO_WORLD_SERVER_PORT);
	int server_socket = socket(PF_INET,SOCK_STREAM,0);
	if( server_socket < 0)
	{
        	sys_log("Create Socket Failed!");
        	return -1;
   	}
	int opt =1;
	setsockopt(server_socket,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));
	if( bind(server_socket,(struct sockaddr*)&server_addr,sizeof(server_addr)))
    	{
        	sys_log("Server Bind Port : %d Failed!", HELLO_WORLD_SERVER_PORT); 
        	return -1;
    	}
	if( listen(server_socket, LENGTH_OF_LISTEN_QUEUE) )
    	{
        	sys_log("Server Listen Failed!"); 
        	return -1;
    	}
	
	struct msg *gyr_data = malloc(sizeof(struct msg) + sizeof(struct mpu6050_data));
	if(gyr_data == NULL){
		sys_log("malloc mem failed");
		return -1;
	}
	int gyr_fd = fifo_open("./gyr_fifo");
	if(gyr_fd < 0){
		sys_log("open or create fifo faild");
		return -1;
	}
	
	tv.tv_sec = 50;    //50ms
        tv.tv_usec = 0;
        if(gyr_fd > maxfd)
                maxfd = gyr_fd;
	for(;;){
		struct sockaddr_in client_addr;
        	socklen_t length = sizeof(client_addr);
		int new_server_socket = accept(server_socket,(struct sockaddr*)&client_addr,&length);
		if ( new_server_socket < 0)
        	{
            		printf("Server Accept Failed!\n");
            		break;
       		}
		
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
				memset(&gyr, 0, sizeof(struct mpu6050_data));
        			fifo_rx(gyr_fd, &gyr_data->tty_msg, sizeof(struct msg) + sizeof(struct mpu6050_data)); 
				if((unsigned char)gyr_data->tty_msg.type == 0xe0){
					memcpy(&gyr, &gyr_data->tty_msg.data, sizeof(struct mpu6050_data));
                                	printf("angle = %4.3f\t%4.3f\t%4.3f\t\r\n",gyr.angle[0],gyr.angle[1],gyr.angle[2]);	
					gyr_msg.start = 0x01;
					gyr_msg.version = PROT_VERSION;
					gyr_msg.sign = 0xF0;
					gyr_msg.end = 0xFF;
					gyr_msg.checksum = sizeof(struct msg) + sizeof(struct mpu6050_data);  //37				
					if(send(new_server_socket,(struct msg *)&gyr_data,gyr_msg.checksum,0) < 0){
						sys_log("socket send data faild");
						break;
					}	
				}		                       
                                
                        }
                }
        }
	fifo_close(gyr_fd);
        free(gyr_data);
	return 0;
}
