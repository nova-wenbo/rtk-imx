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

#define addresssize sizeof(struct sockaddr_in)

void init_serervaddr(struct sockaddr_in *paddr, char *ipstr, int port){    
    bzero ((char *) paddr, addresssize); 
    paddr -> sin_family = AF_INET;
    if(ipstr == NULL){
        paddr ->sin_addr.s_addr = htonl(INADDR_ANY);
    }else{
        inet_pton(AF_INET, ipstr, &(paddr->sin_addr.s_addr));
    }
    paddr -> sin_port = htons(port);
    return;
}


int create_socket(int domain, int type)
{
        int sockfd = 0;
        sockfd = socket(domain, type, 0);
        if (sockfd < 0)
         {
                 return -1;
         }
         return sockfd;
}


int bind_socket(int sockfd, const struct sockaddr *net_addr, socklen_t addrlen)
{
    if (sockfd < 0 || net_addr == NULL)
    {   
        return -1; 
    }   
    if (bind(sockfd, net_addr, addrlen) < 0)
    {   
        return -1; 
    }   
    return 0;
}


int listen_socket(int sockfd, int listennum)
{
    if (sockfd < 0)
    {
        return -1;
    }
    if (listen(sockfd, listennum) < 0)
    {
        return -1;
    }
    return 0;

}

int accept_socket(int sockfd, struct sockaddr *paddr, int addrlen)
{
    if(sockfd < 0 || paddr == NULL){
        return -1;
    }
    return accept(sockfd, paddr, &addrlen);
}

int connect_socket(int sockfd, struct sockaddr *paddr, int addrlen)
{
    if(sockfd < 0 || paddr == NULL){
        return -1;
    }
    return connect(sockfd, paddr, addrlen);
}

int setnonblock(int sockfd)
{
    int opt = 0;
    opt = fcntl(sockfd, F_GETFL);
    if(opt < 0)
    {
        return -1;
    }
    opt = opt|O_NONBLOCK;
    if(fcntl(sockfd, F_SETFL, opt) < 0)
    {
        return -1;
    }   
    return 0;
}
