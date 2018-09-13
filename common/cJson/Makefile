
INC = ../../include
LIB = ../../lib
CC=gcc
CFLAGS = -g -Wall -I${INC}
DEBUG_SRC=*.c
DEBUG_LIB=$(LIB)/libcJson.so

CJSON_SRC = *.c
CJSON_LIB = $(LIB)/libcJson.so
#gcc -fPIC -shared *.c -o libtest.so
$(CJSON_LIB) : $(CJSON_SRC)
	$(CC) $(CFLAGS) -fpic -shared $(CJSON_SRC) -o $(CJSON_LIB)
