#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include<netinet/tcp.h>
#include <linux/ip.h>
#include "debuglog.h"
#include "fifo.h"
#include "common.h"
#include "s2j.h"
#include "s2jdef.h"
#include "cJSON.h"

#if 1
static cJSON *struct_to_json_gyr(void* struct_obj) {
    gyr_msg *struct_msg = (gyr_msg *)struct_obj;
    
    /* create Student JSON object */
    s2j_create_json_obj(json_msg);

    /* serialize data to Student JSON object. */
    s2j_json_set_basic_element(json_msg, struct_msg, uchar, start);
    s2j_json_set_basic_element(json_msg, struct_msg, int, version);
    s2j_json_set_basic_element(json_msg, struct_msg, int, sign);
    s2j_json_set_basic_element(json_msg, struct_msg, int, type);
    /* serialize data to Student.Hometown JSON object. */
    s2j_json_set_struct_element(json_data, json_msg, struct_data, struct_msg, struct mpu6050_data, data);
    s2j_json_set_array_element(json_data, struct_data, float, a, 3);
    s2j_json_set_array_element(json_data, struct_data, float, w, 3);
    s2j_json_set_array_element(json_data, struct_data, float, angle, 3);

    s2j_json_set_basic_element(json_msg, struct_msg, int, end);
    /* return Student JSON object pointer */
    return json_msg;
}
#endif

int startup(int _port,const char* _ip)
{
    int sock = socket(AF_INET,SOCK_STREAM,0);
    if(sock < 0)
    {
        perror("socket");
        exit(1);
    }

    struct sockaddr_in local;
    local.sin_family = AF_INET;
    local.sin_port = htons( _port);
    local.sin_addr.s_addr = inet_addr(_ip);
    socklen_t len = sizeof(local);

    if(bind(sock,(struct sockaddr*)&local , len) < 0)
    {
        perror("bind");
        exit(2);
    }

    if(listen(sock, 5) < 0) //允许连接的最大数量为5
    {
        perror("listen");
        exit(3);
    }

    return sock;
}

int judge_connect(int sock)
{
	struct tcp_info info; 
  	int len = sizeof(info); 
  	getsockopt(sock, IPPROTO_TCP, TCP_INFO, &info, (socklen_t *)&len); 
  	if((info.tcpi_state==TCP_ESTABLISHED))  // 则说明未断开  else 断开
		return 1;
	else
		return 0;
}

int main(int argc, char **argv)
{
	fd_set recv_fds;
        int fd_result;
        int maxfd = 0;
        struct timeval tv;
	if(argc != 3)
   	{
        	printf("Usage:%s [loacl_ip] [loacl_port]\n",argv[0]);
        	return 1;
    	}	
	int listen_sock = startup(atoi(argv[2]),argv[1]);//初始化
	struct sockaddr_in remote;
	socklen_t len = sizeof(struct sockaddr_in);
	struct sigaction sa;
	sa.sa_handler = SIG_IGN;
	sigaction( SIGPIPE, &sa, 0);	

	gyr_msg gyr_msg;
	gyr_msg.start = 0x01;
	gyr_msg.version = 0x01;
	gyr_msg.sign = 0xF0;
	gyr_msg.type = 0xE0;
	gyr_msg.end = 0xFF;

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
		int sock = accept(listen_sock, (struct sockaddr*)&remote, &len);
		if(sock < 0)
        	{
            		perror("accept");
            		continue;
        	}	
		while(judge_connect(sock)){
			
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
					memset(&gyr_msg.data, 0, sizeof(struct mpu6050_data));
               				fifo_rx(gyr_fd, &gyr_msg.data, sizeof(struct mpu6050_data));
					struct_to_json_gyr(&gyr_msg);
					printf("a = %4.3f\t%4.3f\t%4.3f\t\r\n",gyr_msg.data.a[0],gyr_msg.data.a[1],gyr_msg.data.a[2]);
					write(sock, &gyr_msg, sizeof(gyr_msg));
					
                        	}
                	}
		}
        }
	fifo_close(gyr_fd);
	return 0;
}
