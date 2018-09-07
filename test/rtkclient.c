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
#define BUFFER_SIZE 1024


int main(int argc, char **argv)
{
	struct sockaddr_in client_addr;
	bzero(&client_addr,sizeof(client_addr)); //把一段内存区的内容全部设置为0
    	client_addr.sin_family = AF_INET;    //internet协议族
    	client_addr.sin_addr.s_addr = htons(INADDR_ANY);//INADDR_ANY表示自动获取本机地址
    	client_addr.sin_port = htons(0);    //0表示让系统自动分配一个空闲端口

	int client_socket = socket(AF_INET,SOCK_STREAM,0);
    	if( client_socket < 0)
    	{
        	sys_log("Create Socket Failed!");
        	return -1;
    	}

	if( bind(client_socket,(struct sockaddr*)&client_addr,sizeof(client_addr)))
    	{
        	sys_log("Client Bind Port Failed!"); 
        	return -1;
    	}	
	struct sockaddr_in server_addr;
	bzero(&server_addr,sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(HELLO_WORLD_SERVER_PORT);
	socklen_t server_addr_length = sizeof(server_addr);
	if(connect(client_socket,(struct sockaddr*)&server_addr, server_addr_length) < 0)
    	{
        	sys_log("Can Not Connect To %s!\n",argv[1]);
        	return -1;
    	}
	int length = 0;
	struct msg *gyr_data = malloc(sizeof(struct msg) + sizeof(struct mpu6050_data));
        if(gyr_data == NULL){
                sys_log("malloc mem failed");
                return -1;
        }
	memset(gyr_data, 0, sizeof(struct msg) + sizeof(struct mpu6050_data));
	while(length = recv(client_socket,gyr_data,sizeof(struct msg) + sizeof(struct mpu6050_data),0)){
		sys_log("recv data successful");
	
	}
	close(client_socket);
	free(gyr_data);
	return 0;
}
