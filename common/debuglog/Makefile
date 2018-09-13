
INC = ../../include
LIB = ../../lib
CC=gcc
CFLAGS = -g -Wall -I${INC}
DEBUG_SRC=debuglog.c
DEBUG_LIB=$(LIB)/libdebuglog.so

$(DEBUG_LIB) : $(DEBUG_SRC)
	$(CC) $(CFLAGS) -fpic -shared $(DEBUG_SRC) -o $(DEBUG_LIB)

