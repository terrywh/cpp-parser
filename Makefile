CXX?=clang++
CXXFLAGS+= -std=c++11 -g -O0

.PHONY: all test clean
all: test.out
test.out: test.cpp keyval_parser.hpp
	${CXX} ${CXXFLAGS} $^ -o $@
test: test.out
	./test.out
clean:
	rm -f ./test.out