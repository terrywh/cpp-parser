CFLAGS?= -O0 -g -fPIC

all: kv_parser.o


%.o: %.c
	$(CC) ${CFLAGS} -c $^ -o $@
libkv_parser.so: kv_parser.o
	$(CC) -shared -o libkv_parser.so kv_parser.o
libkv_parser.a: kv_parser.o
	$(AR) rcs $@ $^

clean:
	rm -f *.o *.a *.so
kv_parser_test: kv_parser.o kv_parser_test.o
	$(CC) $^ -o $@
test: kv_parser_test
	./kv_parser_test
