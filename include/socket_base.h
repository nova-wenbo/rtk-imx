#ifndef __SOCKET_BASE_H__
#define __SOCKET_BASE_H__
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <netinet/tcp.h>

struct sockaddr_in *ready_socket(const char *ip, int port);
char * getipaddress(struct sockaddr * paddr);
char * tostring(struct sockaddr *paddr);
int getport(struct sockaddr_in *paddr);
int get_listen_socket();
int listen_socket_bind(int sockfd, struct sockaddr * msockaddr);
int sendto(int sockfd, const char* pbuffer, int len, struct sockaddr_in * addr_ptr);
int recvfrom(int sockfd, char* pbuffer, int len, struct sockaddr * msockaddr);
bool isconnected(int sockfd);
bool listen_socket_listen(int sockfd);
bool accept_socket(int sockfd);
bool connect(int sockfd, const char* hostaddr, const int port);
bool getipaddress(int sockfd, struct sockaddr_in msockaddr_in, char* hwaddr);
bool getsockname(int sockfd, char* ip, int* port);
void set_non_blocking(int sockfd, const bool b);
int setKeepAlive(int sockfd, int keepAlive, int keepIdle, int keepInterval, int keepCount);

#endif
