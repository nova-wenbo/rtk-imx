#include "socket_base.h"

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <netinet/tcp.h>

struct sockaddr_in *ready_socket(const char *ip, int port){
    struct  sockaddr_in * paddr;;
    paddr -> sin_family = AF_INET;
    paddr -> sin_port = htons(port);
    paddr -> sin_addr.s_addr = htonl(INADDR_ANY);
    return paddr;
}

char * getipaddress(struct sockaddr * paddr){
	 static char str[40];
	 struct sockaddr *sa = (struct sockaddr *)pAddr;

	 //socklen_t salen = sizeof(_addr);

	switch (sa->sa_family)
	{
		case AF_INET:
		{
			struct sockaddr_in *sin = (struct sockaddr_in *)sa;
			if (inet_ntop(AF_INET, &sin->sin_addr, str, sizeof(str))==NULL)
			{
				return NULL;
			}
			return str;
		}
		break;
		case AF_INET6:
		{
			struct sockaddr_in6 *sin = (struct sockaddr_in6 *)sa;
			if (inet_ntop(AF_INET6, &sin->sin6_addr, str, sizeof(str))==NULL)
			{
				return NULL;
			}

			return str;
		}
		break;
		default:
		return NULL;
	}
}

char * tostring(struct sockaddr *paddr){
	 char portstr[7];
	 struct sockaddr *sa = paddr;
     char stripandport[128];
	 switch (sa->sa_family)
	 {
         case AF_INET:
		 {  
			struct sockaddr_in *sin = (struct sockaddr_in *)sa;
			if (inet_ntop(AF_INET, &sin->sin_addr, stripandport, sizeof(stripandport))==NULL)
			{
				return NULL;
			}
			if (ntohs(sin->sin_port) != 0)
			{
				snprintf(portstr, sizeof(portstr), ":%d", ntohs(sin->sin_port));
				strcat(stripandport, portstr);
			}

			return stripandport;
	   }
	   case AF_INET6:
	   {
			struct sockaddr_in6 *sin = (struct sockaddr_in6 *)sa;
			if (inet_ntop(AF_INET6, &sin->sin6_addr, stripandport, sizeof(_strIpAndPort))==NULL)
			{
				return NULL;
			}
			if (ntohs(sin->sin6_port)!=0)
			{
				snprintf(portstr, sizeof(portstr), ":%d", ntohs(sin->sin6_port));
				strcat(stripandport, portstr);
			}

			return stripandport;
		}
		default:
		{
			return NULL;
		}
	}
}

int getport(struct sockaddr_in *paddr){
    int port = ntohs(paddr -> sin_port);
    return port;
}

int get_listen_socket(){
    int on = 1;
    int sockfd;
    sockfd = socket(AF_INET, SOCKET_STREAM, 0);
    if(socketfd < 0){
        return -1;
    }
    if ( setsockopt (sockfd, SOL_SOCKET, SO_REUSEADDR, ( const char* ) &on, sizeof ( on ) ) == -1 )
    {
        return -1;
    }
    return sockfd;    
}

int listen_socket_bind(int sockfd, struct sockaddr * msockaddr){
	int ret = 0;

	ret = bind(sockfd, msockaddr, sizeof(struct sockaddr_in));

	return ret;
}

int sendto(int sockfd, const char* pbuffer, int len, struct sockaddr_in * addr_ptr){

	int ret = sendto(sockfd, pBuffer, len, 0, (struct sockaddr*)addr_ptr, sizeof(struct sockaddr_in));

	return ret;
}

int recvfrom(int sockfd, char* pbuffer, int len, struct sockaddr * msockaddr){
	int addrlen = sizeof(struct sockaddr_in);
	int ret = recvfrom(sockfd, pbuffer, len, 0, msockaddr, (socklen_t*)&addrlen);

	return ret;
}

bool isconnected(int sockfd){
	if(sockfd<=0)
	{
		return false;
	}

	struct tcp_info info;
	int len=sizeof(info);
	getsockopt(sockfd, IPPROTO_TCP, TCP_INFO, &info, (socklen_t *)&len);
	if((info.tcpi_state == 1)) //TCP_ESTABLISHED = 1
	{
		return true;
	}

	return false;
}

bool listen_socket_listen(int sockfd){
    if(sockfd < 0)
    {
        return false;
    }

    int listen_return = listen(sockfd, MAXCONNECTIONS);

    if(listen_return == -1)
    {
        return false;
    }

    return true;
}

bool accept_socket(int sockfd){
    int connect_fd;
    struct sockaddr_in client;
    int addr_length = sizeof(struct sockaddr_in);
    connect_fd = accept(sockfd, client ,(socklen_t*)&addr_length);

    if(connect_fd == -1)
    {
        return false;
    }
    else
    {
        return true;
    }
}

bool connect(int sockfd, const char* hostaddr, const int port){
	int status;
    struct sockaddr_in sockaddrin;
    if(sockfd < 0) return false;
    
    sockaddrin.sin_family = AF_INET;
    sockaddrin.sin_port = htons (port);

    status = inet_pton(AF_INET, hostaddr, (void*)&(sockaddrin.sin_addr));
    if(errno == EAFNOSUPPORT) return false;

    status = connect(sockfd, (struct sockaddr*) &aockaddrin, sizeof(struct sockaddr));

    if(status == 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool getipaddress(int sockfd, struct sockaddr_in msockaddr_in, char* hwaddr){
	if(sockfd < 0 ) return false;

	char ipaddr[20] = {0};

	if(0L != inet_ntop( AF_INET, (const void*)&msockaddr_in.sin_addr, &ipaddr[0], 16))
	{
		memcpy(hwaddr, ipaddr,16);
		return true;
	}
	else
	{
		return false;
	}
}

bool getsockname(int sockfd, char* ip, int* port){
	struct sockaddr sock_addr;
	struct sockaddr_in* psockaddr_in = NULL;

	socklen_t namelen;
	if(!getsockname(sockfd, &sock_addr, &namelen))
	{
		psockaddr_in = (struct sockaddr_in*)&sock_addr;
		if(ip != NULL)
		{
			char* pstr = NULL;
			pstr = inet_ntoa(psockaddr_in->sin_addr);
			memcpy(ip, pstr, 16);
		}
		if(port != NULL)
		{
			*port = psockaddr_in->sin_port;
		}

		return true;
	}
	return false;
}

void set_non_blocking(int sockfd, const bool b){
    int opts;

    opts = fcntl(sockfd, F_GETFL);

    if(opts < 0)
    {
        return;
    }

    if(b)
        opts = (opts | O_NONBLOCK);
    else
        opts = (opts & ~O_NONBLOCK);

    fcntl(sockfd, F_SETFL,opts);
}

int setKeepAlive(int sockfd, int keepAlive, int keepIdle, int keepInterval, int keepCount){
	int ret = 0;

	ret != setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, (void*)&keepAlive, sizeof(int));
	ret != setsockopt(sockfd, SOL_TCP, TCP_KEEPIDLE, (void*)&keepIdle, sizeof(int));
	ret != setsockopt(sockfd, SOL_TCP, TCP_KEEPINTVL, (void*)&keepInterval, sizeof(int));
	ret != setsockopt(sockfd, SOL_TCP, TCP_KEEPCNT, (void*)&keepCount, sizeof(int));

	return ret;
}
