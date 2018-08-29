CC = gcc
 
ROOT := $(shell pwd)/../common
INCLUDE := $(ROOT)/include
 
CFLAGS += -I$(INCLUDE)
 
target:moda
 
modb:moda.o
	$(CC) -o moda moda.o
 
modb.o:moda.c
	$(CC) $(CFLAGS) -c moda.c 
	
clean:
	-rm *.o moda

