.DEFAULT_GOAL := all
.PHONY: test build clean all

CC=gcc
CFLAGS=-std=c99
OPTIMISE=
LIB=saxbospiral.h

saxbospiral.o: saxbospiral.c $(LIB)
	$(CC) $(CFLAGS) $(OPTIMISE) -c saxbospiral.c

tests.o: $(LIB) tests.c
	$(CC) $(CFLAGS) $(OPTIMISE) -c tests.c

tests: saxbospiral.o tests.o
	$(CC) $(CFLAGS) $(OPTIMISE) -g -o tests saxbospiral.o tests.o

prepare.o: $(LIB) prepare.c
	$(CC) $(CFLAGS) $(OPTIMISE) -c prepare.c

prepare: saxbospiral.o prepare.o
	$(CC) $(CFLAGS) $(OPTIMISE) -g -o prepare saxbospiral.o prepare.o

generate.o: $(LIB) generate.c
	$(CC) $(CFLAGS) $(OPTIMISE) -c generate.c

generate: saxbospiral.o generate.o
	$(CC) $(CFLAGS) $(OPTIMISE) -g -o generate saxbospiral.o generate.o

render.o: $(LIB) render.c
	$(CC) $(CFLAGS) $(OPTIMISE) -c render.c

render: saxbospiral.o render.o
	$(CC) $(CFLAGS) $(OPTIMISE) -g -o render saxbospiral.o render.o -lpng

test: tests
	./tests

build: prepare generate render

clean:
	rm -f *.o tests prepare generate render

all: test build
