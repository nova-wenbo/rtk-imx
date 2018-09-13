
INC = ../include
LIB = ../lib
OBJ = ../obj
RTKC_LIB = $(LIB)/libcJson.so \
	$(LIB)/libdebuglog.so\
	$(LIB)/libfifo.so
		
CC=gcc
RTKC_OBJ=$(OBJ)/rtkc
CFLAGS = -g -Wall -I${INC}
RTKC_SRC=rtkclient.c
$(RTKC_OBJ) : $(RTKS_SRC)
	$(CC) $(CFLAGS) $(RTKC_SRC) $(RTKC_LIB) -lm -o $(RTKC_OBJ) 


