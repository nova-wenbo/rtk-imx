
INC = ../../include
LIB = ../../lib
CC=gcc
CFLAGS = -g -Wall -I${INC}
DEBUG_SRC=base64.c\
	intlib.c\
	sha1.c
DEBUG_LIB=$(LIB)/libwebsocket.so
#base64.c  base64.h  intlib.c  intlib.h  main.c  sha1.c  sha1.h
$(DEBUG_LIB) : $(DEBUG_SRC)
	$(CC) $(CFLAGS) -fpic -shared $(DEBUG_SRC) -o $(DEBUG_LIB)

