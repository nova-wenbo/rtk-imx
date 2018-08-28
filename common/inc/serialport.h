#ifndef __SERIAL_PORT_H__
#define __SERIAL_PORT_H__

#define TRACE_ERR printf

extern int open(const char * fdev, int baudrate, u8 databits, u8 stopbits, char parbit);
extern int read(u8* pbuff, int len);
extern int write(const u8* pbuff, int len);
extern int close(int fd);
#endif
~          
