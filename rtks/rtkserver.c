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
#include <sys/epoll.h>
#include "gps.h"
#include "base64.h"
#include "sha1.h"
#include "intlib.h"
#include "debuglog.h"
#include "fifo.h"
#include "common.h"
#include "s2j.h"
#include "s2jdef.h"
#include "cJSON.h"

#define MAX_EVENTS              1024
#define PER_LINE_MAX            256 
#define REQUEST_LEN_MAX         2048
#define DEFEULT_SERVER_PORT     8000
#define WEB_SOCKET_KEY_LEN_MAX  256 
#define RESPONSE_HEADER_LEN_MAX 2048

/*
 * Function Name: extract_client_key 
 * Description: 提取客户端发送的handshake key值
 * Input Args: @buffer 客户端发送的握手数据
 * Output Args: 输出客户端发来handshake key
 * Return Value: server_key 客户端发来的handshake key
 */
static char *extract_client_key(const char * buffer)
{
    char    *key = NULL;	//保存key值
    char    *start = NULL; 	// 要提取字符串的起始地址 
    char    *flag = "Sec-WebSocket-Key: ";
    int     i = 0;			//初始化循环使用变量
    int     buf_len = 0;	//初始化buffer长度
 
    if(NULL == buffer) {	
        printf("buffer is NULL.\n");
        return NULL;
    }
 
    key=(char *)malloc(WEB_SOCKET_KEY_LEN_MAX);	//分配内存
    if (NULL == key) {
        printf("key alloc failure.\n");
        return NULL;
    }
    memset(key,0, WEB_SOCKET_KEY_LEN_MAX);		//将key重置为0
 
 
    start = strstr(buffer, flag);	//获取flag在buffer中的起始位置
    if(NULL == start) {
        printf("start is NULL.\n");
        return NULL;
    }
 
    start += strlen(flag);		//将指针移至key起始位置
    buf_len = strlen(buffer);	//获取buffer长度
    for(i=0;i<buf_len;i++) {
        if(start[i]==0x0A || start[i]==0x0D) // 当遇到回车换行标志时退出
            break;
        key[i] = start[i];		//保存key值
    }
    return key;	//返回key值
} /* ----- End of extract_client_key()  ----- */

/*
 * Function Name: calculate_accept_key
 * Description: 计算handshake key值
 * Input Args: @buffer 客户端发送的握手数据
 * Output Args: 输出服务器发送的handshake key
 * Return Value: server_key 服务器发送的handshake key
 */
static char *calculate_accept_key(const char * buffer)
{
    int         i;			//初始化循环使用变量
    int         n;			//用于保存数据长度
    char        *client_key = NULL;	//客户端发来的key
    char        *server_key = NULL; 	//返回客户端的key
    char        *sha1_data_tmp = NULL;	//sha1加密后的key
    char        *sha1_data = NULL;	//转换为10进制的加密key
    const char  *guid = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"; //guid存放于.rodata段 
 
    if(NULL == buffer) {			//判断数据是否为NULL
        printf("buffer is NULL.\n");
        return NULL;
    }
    client_key = (char *)malloc(PER_LINE_MAX);	//给client_key分配内存
    if (NULL == client_key) {			//判断内存是否分配成功
        printf("client_key alloc failure.\n");
        return NULL;
    }
    memset(client_key, 0, PER_LINE_MAX);	//重置client_key的值为0
    client_key = extract_client_key(buffer); 	//提取客户端发来的key值
    if(NULL == client_key) {			//判断key值是否为NULL
        printf("client_key is NULL.");
        return NULL;
    }
    strcat(client_key, guid); 			//给client_key添加guid 
    sha1_data_tmp = sha1_hash(client_key); 	//sha1加密client_key 
    n = strlen(sha1_data_tmp);			//获取加密后的key值长度
 
    sha1_data=(char *)malloc(n/2+1);		//给sha1_data分配内存
    if (NULL == sha1_data){			//判断内存是否分配成功
        printf("sha1_data alloc failure.\n");
        return NULL;
    }
    memset(sha1_data,0,n/2+1);				//将sha1_data重置为0
 
    for(i=0; i<n; i+=2)
        sha1_data[i/2] = htoi(sha1_data_tmp, i, 2); 	//将sha1加密后的16进制key值转换为10进制
 
    server_key = base64_encode(sha1_data, n/2);		//将10进制key值转换为base64编码格式
 
    if (client_key != NULL) {	
        free(client_key);		//释放client_key所占内存
        client_key = NULL;		//将client_key转换为NULL指针
    }
 
    if (sha1_data != NULL) {
        free(sha1_data);		//释放sha1_data所占内存
        sha1_data = NULL;		//将sha1_data转换为NULL指针
    }
 
    if (sha1_data_tmp != NULL) {
        free(sha1_data_tmp);	    	//释放sha1_data_tmp所占内存
        sha1_data_tmp = NULL;		//将sha1_data_tmp转换为NULL指针
    }
 
    return server_key;
} /* ----- End of calculate_accept_key()  ----- */

/*
 * Function Name: websocket_shakehand
 * Description: 服务器发送握手消息
 * Input Args: @conn_fd 连接句柄
 * @server_key 服务器加密后的handshake key
 * Output Args: 无
 * Return Value: 无
 */
static void websocket_shakehand(int conn_fd, const char *server_key)
{
    char response_header[RESPONSE_HEADER_LEN_MAX];				//声明response包大小
 
    if(!conn_fd) {								//判断conn_fd是否为空
        printf("connfd is error.\n");
        return ;
    }
 
    if(NULL == server_key) {							//判断server_key是否为空
        printf("server_key is NULL.\n");
        return ;
    }
 
    memset(response_header,'\0',RESPONSE_HEADER_LEN_MAX);			//将response_header重置为空
 
    sprintf(response_header, "HTTP/1.1 101 Switching Protocols\r\n");		//拼接response_header
    sprintf(response_header, "%sUpgrade: websocket\r\n", response_header);
    sprintf(response_header, "%sConnection: Upgrade\r\n", response_header);
    sprintf(response_header, "%sSec-WebSocket-Accept: %s\r\n\r\n", response_header, server_key);
 
    printf("Response Header:%s\n", response_header);
 
    /*
    * write()向套接字中写入数据
    * conn_fd为要写入的文件的描述符
    * response_header为要写入的数据的缓冲区地址
    * strlen(response_header)为要写入的数据的字节数
    */
    write(conn_fd, response_header, strlen(response_header));	
} /* ----- End of websocket_shakehand()  ----- */

/*
 * Function Name: deal_data 
 * Description: 处理客户端数据
 * Input Args: @buffer 接收的数据
 * @buf_len 接收的数据长度
 * Output Args: 无
 * Return Value: payload_data 返回负载数据
 */
char *deal_data(const char *buffer,const int buf_len)
{
    int             i = 0; 						//循环使用
    char            mask_flag;						//mask标志
    char            masks[4];						//数据掩码
    char            *payload_data = NULL;				//存储数据
    char            temp[8];						//转换数据长度用
    unsigned long   n;							//转换数据长度用
    unsigned long   payloadLen = 0;					//数据长度
    if((buffer[0]+128) == 0x8){						//判断opcode是否为0x8
	printf("client is close.\n");		
        return NULL;							//opcode为0x8则关闭连接
    }
	
	
    mask_flag = (buffer[1] & 0x80) == 0x80; 				//获取mask标志位(buffer第9位)
    if (!mask_flag) {							//判断mask标志是否为0,0没有掩码,1有掩码
        printf("no mask.\n");
        return NULL;							//没有掩码则丢弃
    }
 
    payloadLen = buffer[1] & 0x7F; 					//获取payloadLen数值
    if (payloadLen == 126) {      					//如果payloadLen为126则真实长度为buffer第3,4字节
        memcpy(masks, buffer+4, 4);      				//获取掩码(payloadLen结束后跟4字节mask)
        payloadLen =(buffer[2]&0xFF) << 8 | (buffer[3]&0xFF);		//(将buffer第3字节与0xFF进行与运算)后左移8位在进行或运算(buffer第4字节与0xFF进行与运算)
        payload_data=(char *)malloc(payloadLen);			//给payload_data分配内存
        memset(payload_data,0,payloadLen);				//将payload_data重置为0
        memcpy(payload_data,buffer+8,payloadLen);			//获取buffer第8(2+2+4)字节之后的内容(数据部分)
    } else if (payloadLen == 127) {					//如果payloadLen为126则真实长度为buffer第3-10字节
        memcpy(masks,buffer+10,4);  					//获取掩码(payloadLen结束后跟4字节mask)
        for ( i = 0; i < 8; i++)			
            temp[i] = buffer[9 - i];					//获取buffer数据长度(第3-10字节)
 
        memcpy(&n,temp,8);  						//将数据长度赋值给n
        payload_data=(char *)malloc(n); 				//给payload_data分配内存
        memset(payload_data,0,n); 					//将payload_data重置为0
        memcpy(payload_data,buffer+14,n);				//将buffer第14(2+8+4)字节之后的n字节内容赋值给payload_data
        payloadLen=n;    						//设置payloadLen为n
    } else {   								//如果payloadLen为0-125则payloadLen为真实数据长度
        memcpy(masks,buffer+2,4);    					//获取掩码(payloadLen结束后跟4字节mask)
        payload_data=(char *)malloc(payloadLen);			//给payload_data分配内存
        memset(payload_data,0,payloadLen);				//将payload_data重置为0
        memcpy(payload_data,buffer+6,payloadLen); 			//将buffer第6(2+4)字节之后的n字节内容赋值给payload_data
    }
 
    for (i = 0; i < payloadLen; i++)
        payload_data[i] = (char)(payload_data[i] ^ masks[i % 4]);	//将数据与掩码进行异或运算,获得原始数据
 
    printf("data(%ld):%s\n\n", payloadLen, payload_data);
    return payload_data;
} /* ----- End of deal_data()  ----- */

/*
 * Function Name: construct_packet_data
 * Description: 组建websocket数据包
 * Input Args: @message 发送的数据
 * @len 发送数据长度
 * Output Args: 无
 * Return Value: data 返回组建后的包
 */
static char *construct_packet_data(const char *message, unsigned long *len)
{
    char *data = NULL;
    unsigned long n;
 
    if (NULL == message) {				//判断message是否为空
        printf("message is NULL.\n");
        return NULL;
    }
 
    n = strlen(message);				//获取message长度
    if (n < 126) {					//判断n是否小于126,小于126则payload len长度位7位
        data=(char *)malloc(n+2);			//给data分配内存
        if (NULL == data) {				//判断data是否为NULL
            printf("data is NULL.\n");
            return NULL;
        }
        memset(data,0,n+2);	 			//重置data为0
        data[0] = 0x81;					//设置第0-7位为1000 0001(FIN为1,Opcode为1)
        data[1] = n;					//设置第8位为0,9-15位为n(第8位为mask,9-15位为数据长度,客户端发送mask为1,服务端发送mask为0)
        memcpy(data+2,message,n);			//将message添加到第2个字节之后
        *len=n+2;					//将指针指向message首地址
    } else if (n < 0xFFFF) {				//当n小于0xFFFF则为126,后2字节为真实长度
        data=(char *)malloc(n+4);			//给data分配内存
        if (NULL == data) {				//判断data是否为NULL
            printf("data is NULL.\n");
            return NULL;
        }
        memset(data,0,n+4);				//重置data为0
        data[0] = 0x81;					//设置第0-7位为1000 0001(FIN为1,Opcode为1)
        data[1] = 126;					//设置第8-15位为0111 1110
        data[2] = (n>>8 & 0xFF);			//设置第16-23位为n-128(将n右移8位在与1111 1111做与运算)
        data[3] = (n & 0xFF);				//设置第24-31位为n的右8(0-7)位
        memcpy(data+4,message,n);    			//将message添加到第4个字节之后
        *len=n+4;					//将指针指向message首地址
    } else {						//当n大于0xFFFF则payload len前7位为127,后8字节为真实长度
        data=(char *)malloc(n+10);			//给data分配内存
        if (NULL == data) {				//判断data是否为NULL
            printf("data is NULL.\n");
            return NULL;
        }
        memset(data,0,n+10);				//重置data为0
        data[0] = 0x81;					//设置第0-7位为1000 0001(FIN为1,Opcode为1)
        data[1] = 127;					//设置第8-15位为0111 1111
        data[2] = (n>>56 & 0xFF);			//设置第16-23位为n-128(将n右移8位在与1111 1111做与运算)
		data[3] = (n>>48 & 0xFF);		//设置第24-31位为n-128(将n右移8位在与1111 1111做与运算)
		data[4] = (n>>40 & 0xFF);		//设置第32-39位为n-128(将n右移8位在与1111 1111做与运算)
		data[5] = (n>>32 & 0xFF);		//设置第40-47位为n-128(将n右移8位在与1111 1111做与运算)
		data[6] = (n>>24 & 0xFF);		//设置第48-55位为n-128(将n右移8位在与1111 1111做与运算)
		data[7] = (n>>16 & 0xFF);		//设置第56-63位为n-128(将n右移8位在与1111 1111做与运算)
		data[8] = (n>>8 & 0xFF);		//设置第64-71位为n-128(将n右移8位在与1111 1111做与运算)
        data[9] = (n & 0xFF);				//设置第72-79位为n的右8(0-7)位
        memcpy(data+10,message,n);    			//将message添加到第10个字节之后
        *len=n+10;					//将指针指向message首地址
    }
 
    return data;
} /* ----- End of construct_packet_data()  ----- */

/*
 * Function Name: response
 * Description: 响应客户端
 * Input Args: @conn_fd 连接句柄
 * @message 发送的数据
 * Output Args: 无
 * Return Value: 无
 */
void response(int conn_fd, const char *message)
{
    char *data = NULL;
    unsigned long n=0;
 
    if(!conn_fd) {					//判断套接字是否错误
        printf("conn_fd is error.\n");
        return ;
    }
 
    if(NULL == message) {				//判断message是否为NULL
        printf("message is NULL111.\n");
        return ;
    }
    data = construct_packet_data(message, &n); 		//传入message获取data(数据段)
    //data = construct_packet_data("hello wenbo , nizuiniubi !!!", &n);
    if(NULL == data || n <= 0)				//判断data是否为NULL或数据长度是否为0
    {
        printf("data is empty!\n");
        return ;
    } 
 
    write(conn_fd, data, n);				//将数据写入套接字
    printf("successful \n"); 
    if (NULL != data) {					//如果data不为NULL则释放内存并赋值为NULL
        free(data);				
        data = NULL;
    }
} /* ----- End of response()  ----- */


#if 1
static cJSON *struct_to_json_gyr(void* struct_obj) {
    gyr_msg *struct_msg = (gyr_msg *)struct_obj;
    
    /* create Student JSON object */
    s2j_create_json_obj(json_msg);

    /* serialize data to Student JSON object. */
    s2j_json_set_basic_element(json_msg, struct_msg, uchar, start);
    s2j_json_set_basic_element(json_msg, struct_msg, uchar, version);
    s2j_json_set_basic_element(json_msg, struct_msg, uchar, sign);
    s2j_json_set_basic_element(json_msg, struct_msg, uchar, type);
    /* serialize data to Student.Hometown JSON object. */
    s2j_json_set_struct_element(json_data, json_msg, struct_data, struct_msg, struct mpu6050_data, data);
    s2j_json_set_array_element(json_data, struct_data, float, a, 3);
    s2j_json_set_array_element(json_data, struct_data, float, w, 3);
    s2j_json_set_array_element(json_data, struct_data, float, angle, 3);

    s2j_json_set_basic_element(json_msg, struct_msg, uchar, end);
    /* return Student JSON object pointer */
    return json_msg;
}
static cJSON *struct_to_json_gps(void* struct_obj)
{
	gps_info *struct_msg = (gps_info *)struct_obj;
	s2j_create_json_obj(json_msg);
	s2j_json_set_basic_element(json_msg, struct_msg, double, latitude);
	s2j_json_set_basic_element(json_msg, struct_msg, double, longitude);
	s2j_json_set_basic_element(json_msg, struct_msg, int, latitude_Degree);
	s2j_json_set_basic_element(json_msg, struct_msg, int, latitude_Cent);
	s2j_json_set_basic_element(json_msg, struct_msg, int, latitude_Second);
	s2j_json_set_basic_element(json_msg, struct_msg, int, longitude_Degree);
	s2j_json_set_basic_element(json_msg, struct_msg, int, longitude_Cent);
	s2j_json_set_basic_element(json_msg, struct_msg, int, longitude_Second);
	s2j_json_set_basic_element(json_msg, struct_msg, float, speed); //float
	s2j_json_set_basic_element(json_msg, struct_msg, float, direction);//float
	s2j_json_set_basic_element(json_msg, struct_msg, float, height);//float
	s2j_json_set_basic_element(json_msg, struct_msg, int, satellite);
	s2j_json_set_basic_element(json_msg, struct_msg, int, NS); 
	s2j_json_set_basic_element(json_msg, struct_msg, int, EW); 
	s2j_json_set_struct_element(json_data, json_msg, struct_data, struct_msg, DATE_TIME, D);
	s2j_json_set_basic_element(json_data, struct_data, int, year);
	s2j_json_set_basic_element(json_data, struct_data, int, month);
	s2j_json_set_basic_element(json_data, struct_data, int, day);	
	s2j_json_set_basic_element(json_data, struct_data, int, hour);
	s2j_json_set_basic_element(json_data, struct_data, int, minute);	
	s2j_json_set_basic_element(json_data, struct_data, int, second);
	return json_msg;
}

static cJSON *struct_to_json_tmp(void* struct_obj) {
    temp_msg *struct_msg = (temp_msg *)struct_obj;

    /* create Student JSON object */
    s2j_create_json_obj(json_msg);

    /* serialize data to Student JSON object. */
    s2j_json_set_basic_element(json_msg, struct_msg, uchar, start);
    s2j_json_set_basic_element(json_msg, struct_msg, uchar, version);
    s2j_json_set_basic_element(json_msg, struct_msg, uchar, sign);
    s2j_json_set_basic_element(json_msg, struct_msg, uchar, type);
    /* serialize data to Student.Hometown JSON object. */
    s2j_json_set_basic_element(json_msg, struct_msg, uchar, data);
    s2j_json_set_basic_element(json_msg, struct_msg, uchar, end);
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
	int                 n;
    	int                 conn_fd;
    	int                 listen_fd; 
    	int                 port = DEFEULT_SERVER_PORT;	
    	char                *data = NULL;
    	char                buf[REQUEST_LEN_MAX];	
    	char                str[INET_ADDRSTRLEN];
    	char                *sec_websocket_key = NULL;
    	struct sockaddr_in  servaddr;
    	struct sockaddr_in  cliaddr;
    	socklen_t           cliaddr_len;
	

	if(argc > 1) 			
		port = atoi(argv[1]);			
	if(port<=0 || port>0xFFFF) {			
		printf("Port(%d) is out of range(1-%d)\n", port, 0xFFFF);
    		return -1;
	}

    	listen_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(listen_fd == -1){
		printf("create socket faild !!!");
        	return -1;
    	}
	memset(&servaddr, 0, sizeof(servaddr));
    	servaddr.sin_family = AF_INET;			
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);	
    	servaddr.sin_port = htons(port);		
	bind(listen_fd, (struct sockaddr *)&servaddr, sizeof(servaddr));
 	listen(listen_fd, 50);						
	cliaddr_len = sizeof(cliaddr);			
	printf("Listen %d\nAccepting connections ...\n",port); 
	int epoll_fd=epoll_create(MAX_EVENTS);  	
    	if(epoll_fd==-1)  					
    	{  
       		perror("epoll_create failed");  
        	exit(EXIT_FAILURE);  
    	}  
	struct epoll_event ev;				 
    	struct epoll_event events[MAX_EVENTS];	  
    	ev.events=EPOLLIN;				
    	ev.data.fd=listen_fd;  					
	if(epoll_ctl(epoll_fd,EPOLL_CTL_ADD,listen_fd,&ev)==-1)	//注册新的listen_fd到epoll_fd中  
    	{  
        	perror("epll_ctl:servaddr register failed");  
        	exit(EXIT_FAILURE);  
    	}  
    	int nfds;	// epoll监听事件发生的个数  
	gps_info gps;
	/* select listen*/	
	fd_set recv_fds;
        int fd_result;
        int maxfd = 0;
        struct timeval tv;
	/* protocol packaging*/
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
	int gps_fd = fifo_open("./gps_fifo");
	if(gps_fd < 0){
		sys_log("open or create fifo faild");
                return -1;
	}
	temp_msg tmp_msg;
	gyr_msg.start = 0x01;
        gyr_msg.version = 0x01;
        gyr_msg.sign = 0xF0;
        gyr_msg.type = 0xE1;
        gyr_msg.end = 0xFF;
	int tmp_fd = fifo_open("./tmp_fifo");
        if(tmp_fd < 0){
                sys_log("open or create fifo faild");
                return -1;
        }

	cJSON *cjson_gyr =  cJSON_CreateObject();
	cJSON *cjson_tmp =  cJSON_CreateObject();
	cJSON *cjson_gps =  cJSON_CreateObject();
	tv.tv_sec = 3600;    //3600s
        tv.tv_usec = 0;
        if(gyr_fd > maxfd)
                maxfd = gyr_fd;
	if(gps_fd > maxfd)
		maxfd = gps_fd;
	if(tmp_fd > maxfd)
		maxfd = tmp_fd;
	for(;;){
		nfds=epoll_wait(epoll_fd,events,MAX_EVENTS,20); // 等待事件发生
		if(nfds==-1)  
        	{  
            		perror("start epoll_wait failed");  
            		exit(EXIT_FAILURE);  
        	}  
		int i; 
		for(i=0;i<nfds;i++)  
        	{  
            		if(events[i].data.fd==listen_fd)	// 客户端有新的连接请求    
            		{      
			/*
			* 等待客户端连接请求到达,accept()函数监听客户端请求
			* listen_fd 为服务端套接字,cliaddr为sockaddr_in结构体变量,cliaddr_len为参数cliaddr的长度,可由sizeof()求得
	  		* accept()返回一个新的套接字来和客户端通信,cliaddr保存了客户端的IP地址和端口号,
			* listen_fd是服务器端的套接字,后面和客户端通信时，要使用cliaddr这个新生成的套接字，而不是原来服务器端的套接字
			*/
                		if((conn_fd=accept(listen_fd, (struct sockaddr *)&cliaddr, &cliaddr_len))<0)  
                		{     
                    			perror("accept conn_fd failed");     
                    			exit(EXIT_FAILURE);  
                		}  
                		ev.events=EPOLLIN;
				ev.data.fd=conn_fd; 
                		if(epoll_ctl(epoll_fd,EPOLL_CTL_ADD,conn_fd,&ev)==-1)  
                		{  
                    			perror("epoll_ctl:conn_fd register failed");  
                    			exit(EXIT_FAILURE);  
                		}  
				memset(buf, 0, REQUEST_LEN_MAX);
                		printf("From %s at PORT %d\n", inet_ntop(AF_INET, &cliaddr.sin_addr, str, sizeof(str)),ntohs(cliaddr.sin_port));	
				n = read(conn_fd, buf, REQUEST_LEN_MAX);	
				printf("-------------------------------------------------------------------------------\n");
				printf("read:%d\n%s\n", n, buf); 
				sec_websocket_key = calculate_accept_key(buf);	//传入客户握手buf数据,计算服务器发送的握手key
				websocket_shakehand(conn_fd, sec_websocket_key);//发送服务端握手信息
				if (sec_websocket_key != NULL) {	       //如果sec_websocket_key(服务端握手key)不为NULL
					free(sec_websocket_key);		//释放sec_websocket_key(服务端握手key)内存
					sec_websocket_key = NULL;		//sec_websocket_key(服务端握手key)赋值为NULL
				}	
            		}else{  // deal from client data 
                		n = read(conn_fd, buf, REQUEST_LEN_MAX); 
                		if(n<0){  			
                    			perror("read from client failed\n");  
                    			exit(EXIT_FAILURE);;			
                		}
            			data = deal_data(buf, n);
				if(data==NULL){	
					close(conn_fd);			
				}				
            		}		
        	}
               	FD_ZERO(&recv_fds);
                FD_SET(gyr_fd, &recv_fds);
		FD_SET(gps_fd, &recv_fds);
		FD_SET(tmp_fd, &recv_fds);
		
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
               	}else {
                        if(FD_ISSET(gyr_fd, &recv_fds)){
				memset(&gyr_msg.data, 0, sizeof(struct mpu6050_data));
               			fifo_rx(gyr_fd, &gyr_msg.data, sizeof(struct mpu6050_data));
				cjson_gyr = struct_to_json_gyr(&gyr_msg);
				//printf("a = %4.3f\t%4.3f\t%4.3f\t\r\n",gyr_msg.data.a[0],gyr_msg.data.a[1],gyr_msg.data.a[2]);
				response(conn_fd, cJSON_Print(cjson_gyr));			
				
                      	}
			if(FD_ISSET(tmp_fd, &recv_fds)){
				fifo_rx(tmp_fd, &tmp_msg.data, sizeof(uchar));
				cjson_tmp = struct_to_json_tmp(&tmp_msg);
				response(conn_fd, cJSON_Print(cjson_tmp));
			}
			if(FD_ISSET(gps_fd, &recv_fds)){
				fifo_rx(gps_fd, &gps,sizeof(gps));
				cjson_gps = struct_to_json_gps(&gps);
				response(conn_fd, cJSON_Print(cjson_gps));
				printf("height : %02f, satellite : %d \n", gps.height, gps.satellite);
                                printf("time : %d-%d-%d-%d:%d:%d \n",gps.D.year,gps.D.month,gps.D.day,gps.D.hour,gps.D.minute,gps.D.second);
                                printf("latitude : %d-%d-%d\n",gps.latitude_Degree,gps.latitude_Cent,gps.latitude_Second);
                                printf("gps: %lf-%lf\n", gps.latitude, gps.longitude);
			}
                }
	}
	fifo_close(gps_fd);
	fifo_close(gyr_fd);
	fifo_close(tmp_fd);
	return 0;
}
