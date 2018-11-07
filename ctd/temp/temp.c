#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include "serialport.h"
#include "debuglog.h"
#include "common.h"
#include "fifo.h"
#include "gps.h"

tty_info *uart_init(int id, int speed)
{
        tty_info *putty = ready_tty(id);
        if(putty == NULL){
                sys_log("ready uart%d faild", id);
                return NULL;
        }
        lock_tty(putty);
        if(set_ttySpeed(putty, speed)>0){
                sys_log("set uart Speed() error");
                return NULL;
        }
        if(set_ttyParity(putty,8,'N',1)>0){
                sys_log("set uart Parity() error");
                return NULL;
        }
        return putty;
}

int main(int argc, char **argv)
{
        unsigned char tmp_send[] = {0x54, 0x50, 0x01, 0xF1, 0x96};
        unsigned char tmp_recv[7] = {0};
        //int i = 0;
        unsigned char data = 0;
        sys_log("tmp");
        tty_info *tmp_tty = uart_init(0, 9600);
        if(tmp_tty == NULL)
                exit(-1);
        int tmp_fd = fifo_open("./tmp_fifo");
        if(tmp_fd < 0){
                sys_log("open or create fifo faild");
                exit(-1);
        }
        for(;;){
                sleep(4);
                int count = sendn_tty(tmp_tty, tmp_send, sizeof(tmp_send));
                if(count < 0)
                        exit(-1);
                sys_log("brfore read");
                usleep(10000);
                count = recvn_tty(tmp_tty, tmp_recv, sizeof(tmp_recv));
                sys_log("tmp read %d", count);
                data = (tmp_recv[4]*256 + tmp_recv[5])/10;
                fifo_tx(tmp_fd, &data, sizeof(uchar));
                printf("tmp : %d\n", data);
        }
        close(tmp_fd);
        clean_tty(tmp_tty);

	return 0;
}
