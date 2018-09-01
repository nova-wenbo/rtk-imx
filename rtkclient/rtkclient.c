#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include "socket_base.h"

#define SERVER_PORT 8000
#define MAXLINE 4096

int main(int agrc, char **argv)
{
    struct sockaddr_in serveraddr;
    struct socket_info msocket_info;
    int confd, len;
    char buf[MAXLINE];
    char command = 'A';
    char ipstr[] = "192.168.164.128";
    init_serervaddr(&serveraddr, ipstr, SERVER_PORT);
    msocket_info.domain = AF_INET;
    msocket_info.type = SOCK_STREAM;
    msocket_info.addrlen = sizeof(serveraddr);
    /*confd = create_socket(AF_INET, SOCK_STREAM);
    connect_socket(confd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));*/
    confd = init_clientsocket((struct sockaddr *)&serveraddr, msocket_info);
    printf("confd is %d \n", confd);
    write(confd, &command, 1);
    len = read(confd, buf, sizeof(buf));
    printf("buf is %c \n", buf[0]);
    close(confd);
    return 0;
}

