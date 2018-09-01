#include <sys/types.h>      
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include "socket_base.h"

#define SERVER_PORT 8000
#define MAXLINE 4096

int main(int argc , char **argv){
    struct socket_info msocket_info;
    struct sockaddr_in serveraddr, clientaddr;
    int sockfd, addrlen, confd, len, i;
    char ipstr[128];
    char buf[MAXLINE];
    bzero(&serveraddr, sizeof(serveraddr));
    msocket_info.domain = AF_INET;
    msocket_info.type = SOCK_STREAM;
    msocket_info.listennum = 128;
    msocket_info.addrlen = sizeof(serveraddr);
    init_serervaddr(&serveraddr, NULL, SERVER_PORT);
    /*sockfd = create_socket(AF_INET, SOCK_STREAM);
    bind_socket(sockfd, (struct sockaddr_in *)&serveraddr, sizeof(serveraddr));
    listen_socket(sockfd, 128);*/
    sockfd = init_serversocket((struct sockaddr_in *)&serveraddr, msocket_info);
    while(1){
        addrlen = sizeof(clientaddr);
        confd = accept_socket(sockfd, (struct sockaddr *)&clientaddr, &addrlen);
        inet_ntop(AF_INET, &clientaddr.sin_addr.s_addr, ipstr, sizeof(ipstr));
        printf("client ip %s\tport %d\n", 
                inet_ntop(AF_INET, &clientaddr.sin_addr.s_addr, ipstr, sizeof(ipstr)),
                ntohs(clientaddr.sin_port));

        len = read(confd, buf, sizeof(buf));
        printf("buf[0] is %c \n", buf[0]);
        buf[0] = buf[0] + 1;
        write(confd, &buf[0], 1);

        close(confd);
    }
    close(sockfd);

    return 0;
}
