
INC = ../../include
LIB = ../../lib
CC=gcc
CFLAGS = -g -Wall -I${INC}
DEBUG_SRC=serialport.c
DEBUG_LIB=$(LIB)/libserialport.so

$(DEBUG_LIB) : $(DEBUG_SRC)
	$(CC) $(CFLAGS) -fpic -shared $(DEBUG_SRC) -o $(DEBUG_LIB)

