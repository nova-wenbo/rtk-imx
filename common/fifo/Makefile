
INC = ../../include
LIB = ../../lib
CC=gcc
CFLAGS = -g -Wall -I${INC}
FIFO_SRC=fifo.c
FIFO_LIB=$(LIB)/libfifo.so
$(FIFO_LIB) : $(FIFO_SRC)
	$(CC) $(CFLAGS) -fpic -shared $(FIFO_SRC) -o $(FIFO_LIB) 

