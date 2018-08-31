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
    int confd, len;
    char buf[MAXLINE];
    char command = 'A';
    char ipstr[] = "10.20.8.240";
    init_serervaddr(&serveraddr, ipstr, SERVER_PORT);
    confd = create_socket(AF_INET, SOCK_STREAM);
    connect_socket(confd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
    write(confd, &command, 1);
    len = read(confd, buf, sizeof(buf));
    printf("buf is %c \n", buf[0]);
    close(confd);
    return 0;
}

