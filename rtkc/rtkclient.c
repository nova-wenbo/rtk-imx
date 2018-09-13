#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<unistd.h>
#include<stdlib.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include <string.h>
#include"common.h"
int main(int argc,const char* argv[])
{
    if(argc != 3)
    {
        printf("Usage:%s [ip] [port]\n",argv[0]);
        return 0;
    }

    //创建一个用来通讯的socket
    int sock = socket(AF_INET,SOCK_STREAM, 0);
    if(sock < 0)
    {
        perror("socket");
        return 1;
    }

    //需要connect的是对端的地址，因此这里定义服务器端的地址结构体
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(atoi(argv[2]));
    server.sin_addr.s_addr = inet_addr(argv[1]);
    socklen_t len = sizeof(struct sockaddr_in);
    if(connect(sock, (struct sockaddr*)&server, len) < 0 )
    {
        perror("connect");
        return 2;
    }
    //连接成功进行收数据
    gyr_msg gyr_msg;;
    while(1)
    {
	#if 0
        printf("send###");
        fflush(stdout);
        ssize_t _s = read(0, buf, sizeof(buf)-1);
        buf[_s] = 0;
        write(sock, buf, _s);
	#endif
	memset(&gyr_msg, 0, sizeof(gyr_msg));
	ssize_t _s = read(sock, &gyr_msg, sizeof(gyr_msg));
            if(_s > 0)
            {
                printf("data : %02x \n",gyr_msg.start);
		printf("a = %4.3f\t%4.3f\t%4.3f\t\r\n",gyr_msg.data.a[0],gyr_msg.data.a[1],gyr_msg.data.a[2]);
            }
            else
            {
                printf("client is quit!\n");
                break;
            }

    }
    close(sock);
    return 0;
}
