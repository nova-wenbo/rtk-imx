#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "debuglog.h"
#include "fifo.h"
#include "common.h"
#include "cJson/s2j.h"
#include "cJson/s2jdef.h"
#include "cJson/cJSON.h"

#if 1
static cJSON *struct_to_json_gyr(void* struct_obj) {
    gyr_msg *struct_msg = (gyr_msg *)struct_obj;
    
    /* create Student JSON object */
    s2j_create_json_obj(json_msg);

    /* serialize data to Student JSON object. */
    s2j_json_set_basic_element(json_msg, struct_msg, uchar, start);
    s2j_json_set_basic_element(json_msg, struct_msg, int, version);
    s2j_json_set_basic_element(json_msg, struct_msg, int, sign);
    s2j_json_set_basic_element(json_msg, struct_msg, int, type);
    /* serialize data to Student.Hometown JSON object. */
    s2j_json_set_struct_element(json_data, json_msg, struct_data, struct_msg, struct mpu6050_data, data);
    s2j_json_set_array_element(json_data, struct_data, float, a, 3);
    s2j_json_set_array_element(json_data, struct_data, float, w, 3);
    s2j_json_set_array_element(json_data, struct_data, float, angle, 3);

    s2j_json_set_basic_element(json_msg, struct_msg, int, end);
    /* return Student JSON object pointer */
    return json_msg;
}
#endif
int main(int argc, char **argv)
{
	fd_set recv_fds;
        int fd_result;
        int maxfd = 0;
        struct timeval tv;

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
	
	tv.tv_sec = 50;    //50ms
        tv.tv_usec = 0;
        if(gyr_fd > maxfd)
                maxfd = gyr_fd;
	for(;;){
                FD_ZERO(&recv_fds);
                FD_SET(gyr_fd, &recv_fds);
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
                }
                else {
                        if(FD_ISSET(gyr_fd, &recv_fds)){
				memset(&gyr_msg.data, 0, sizeof(struct mpu6050_data));
               			fifo_rx(gyr_fd, &gyr_msg.data, sizeof(struct mpu6050_data));
				cJSON *json_msg = struct_to_json_gyr(&gyr_msg);
//				gyr_msg *converted_msg_obj = json_to_struct(json_msg);

//				if(memcmp(&gyr_msg, converted_msg_obj, sizeof(gyr_msg))) {
  //      				printf("Converted failed!\n");
    //				} else {
      //  				printf("Converted OK!\n");
    	//			}
    	//			s2j_delete_json_obj(json_msg);
    	//			s2j_delete_struct_obj(converted_msg_obj);

				//printf("a = %4.3f\t%4.3f\t%4.3f\t\r\n",gyr_msg.data.a[0],gyr_msg.data.a[1],gyr_msg.data.a[2]);
					
                        }
                }
        }
	fifo_close(gyr_fd);
	return 0;
}
