.DEFAULT_GOAL := all
.PHONY: test-unit test-func logo build clean all

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

test-unit: tests
	./tests

test-func: prepare generate render
	@echo "Running Functional Test"
	@echo -n "saxbospiral `git describe --abbrev=0`" > message.hex
	@./prepare message.hex message.sxp.hex
	@./generate message.sxp.hex
	@./render message.sxp.hex saxbospiral_test.png
	@diff saxbospiral.png saxbospiral_test.png
	@rm saxbospiral_test.png message.hex message.sxp.hex

logo: prepare generate render
	@echo "Generating logo"
	@echo -n "saxbospiral `git describe --abbrev=0`" > saxbospiral.hex
	@./prepare saxbospiral.hex saxbospiral.sxp
	@./generate saxbospiral.sxp
	@./render saxbospiral.sxp saxbospiral.png
	@rm saxbospiral.hex saxbospiral.sxp

build: prepare generate render

clean:
	rm -f *.o tests prepare generate render

all: test-unit test-func build
