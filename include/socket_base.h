#ifndef __SOCKET_BASE_H__
#define __SOCKET_BASE_H__
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <netinet/tcp.h>

void init_serervaddr(struct sockaddr_in *paddr, char *ipstr, int port);
int create_socket(int domain, int type);
int bind_socket(int sockfd, const struct sockaddr *net_addr, socklen_t addrlen);
int listen_socket(int sockfd, int listennum);
int accept_socket(int sockfd, struct sockaddr *paddr, int addrlen);
int connect_socket(int sockfd, struct sockaddr *paddr, int addrlen);
int setnonblock(int sockfd);
#endif
