#include <time.h>
#include <stdarg.h>
#include <syslog.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "debuglog.h"
void LOG(const char* ms, ...)
{
	char wzLog[1024] = {0};
	char buffer[1024] = {0};
	va_list args;
	va_start(args, ms);
	vsprintf(wzLog, ms, args);
	va_end(args);
 
	time_t now;
	time(&now);
	struct tm *local;
	local = localtime(&now);
	printf("%04d-%02d-%02d %02d:%02d:%02d %s\n", local->tm_year+1900, local->tm_mon,
			local->tm_mday, local->tm_hour, local->tm_min, local->tm_sec,
			wzLog);
	sprintf(buffer,"%04d-%02d-%02d %02d:%02d:%02d %s\n", local->tm_year+1900, local->tm_mon,
				local->tm_mday, local->tm_hour, local->tm_min, local->tm_sec,
				wzLog);
#ifdef SYS_LOG_FILE
	int file_fd = open("/var/log/debuglog.txt", O_RDWR | O_CREAT | O_APPEND, 0777);
	if(file_fd < 0){
		printf("create or open /var/log/debuglog.txt failed \n");
		return;
	}
	if(write(file_fd, buffer, strlen(buffer)) < 0){
		printf("write /var/log/debuglog.txt failed\n");
		return;
	}
	close(file_fd);
#endif
	//FILE* file = fopen("testResut.log","a+");
	//fwrite(buffer,1,strlen(buffer),file);
	//fclose(file);
 
	return ;
}


