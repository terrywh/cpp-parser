CXX?=clang++
CXXFLAGS+= -std=c++11 -g -O0
PREFIX?=/data/vendor

.PHONY: all test clean install
all: test.out
test.out: test.cpp keyval_parser.hpp
	${CXX} ${CXXFLAGS} $^ -o $@
test: test.out
	./test.out
clean:
	rm -f ./test.out
install: keyval_parser.hpp
	mkdir -p ${PREFIX}/include
	cp -f $^ ${PREFIX}/include/