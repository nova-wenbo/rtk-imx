
INC = /usr/include
LIB = /lib
OBJ = ../obj
CTD_LIB = $(LIB)/libcJson.so \
	$(LIB)/libdebuglog.so\
	$(LIB)/libfifo.so\
	$(LIB)/libserialport.so
		
CC=gcc
CTD_OBJ=gps
CFLAGS = -g -I${INC}
CTD_SRC=main.c gps.c
$(CTD_OBJ) : $(CTD_SRC)
	$(CC) $(CFLAGS) $(CTD_SRC) $(CTD_LIB) -lm -lpthread -o $(CTD_OBJ) 


