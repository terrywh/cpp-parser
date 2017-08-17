CFLAGS?= -O0 -g -fPIC

all: kv_parser.o

kv_parser.o: kv_parser.c kv_parser.h
	$(CC) -c kv_parser.c -o kv_parser.o
kv_parser: kv_parser.o
	$(CC) -shared -o libkv_parser.so kv_parser.o

clean:
	rm -f *.o *.so
