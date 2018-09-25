
INC = /usr/include
LIB = /lib
OBJ = ../obj
FIFO_LIB = $(LIB)/libcJson.so \
	$(LIB)/libdebuglog.so\
	$(LIB)/libfifo.so\
	$(LIB)/libwebsocket.so
		
CC=gcc
RTKS_OBJ=$(OBJ)/rtks
CFLAGS = -g -Wall -I${INC}
RTKS_SRC=rtkserver.c
$(RTKS_OBJ) : $(RTKS_SRC)
	$(CC) $(CFLAGS) $(RTKS_SRC) $(FIFO_LIB) -lm -o $(RTKS_OBJ) 


