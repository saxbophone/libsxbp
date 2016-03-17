.DEFAULT_GOAL := all
.PHONY: test build clean all

CC=gcc
CFLAGS=-std=c99

saxbospiral.o: saxbospiral.c saxbospiral.h
	$(CC) $(CFLAGS) -c saxbospiral.c

tests.o: tests.c
	$(CC) $(CFLAGS) -c tests.c

tests: saxbospiral.o tests.o
	$(CC) $(CFLAGS) -g -o tests saxbospiral.o tests.o

prepare.o: prepare.c
	$(CC) $(CFLAGS) -c prepare.c

prepare: saxbospiral.o prepare.o
	$(CC) $(CFLAGS) -g -o prepare saxbospiral.o prepare.o

generate.o: generate.c
	$(CC) $(CFLAGS) -c generate.c

generate: saxbospiral.o generate.o
	$(CC) $(CFLAGS) -g -o generate saxbospiral.o generate.o

test: tests
	./tests

build: prepare generate

clean:
	rm -f *.o

all: test build
