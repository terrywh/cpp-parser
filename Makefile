CFLAGS?= -O3

all: kv_parser.o


%.o: %.c
	$(CC) ${CFLAGS} -fPIC -c $^ -o $@
libkv_parser.so: kv_parser.o
	$(CC) -shared -o libkv_parser.so kv_parser.o
libkv_parser.a: kv_parser.o
	$(AR) rcs $@ $^

clean:
	rm -f *.o *.a *.so
kv_parser_test: kv_parser.o kv_parser_test.o
	$(CC) $^ -o $@
test: CFLAGS = -O0 -g
test: kv_parser_test
	./kv_parser_test
