.DEFAULT_GOAL := all
.PHONY: test-unit test-func logo build clean all

CC=gcc
STANDARD=-std=c99
OPTIMISE=-O0
DEBUG=-g
INCLUDES=
CFLAGS=$(STANDARD) $(OPTIMISE) $(DEBUG) $(INCLUDES)
LIBPNG=-lpng
OS_NAME=
EXE_SUFFIX=.out

LIB=saxbospiral/

SAXBOSPIRAL=$(LIB)saxbospiral
SOLVE=$(LIB)solve
SERIALISE=$(LIB)serialise

TESTS=tests$(OS_NAME)$(EXE_SUFFIX)
PREPARE=prepare$(OS_NAME)$(EXE_SUFFIX)
GENERATE=generate$(OS_NAME)$(EXE_SUFFIX)
RENDER=render$(OS_NAME)$(EXE_SUFFIX)

$(SAXBOSPIRAL)$(OS_NAME).o: $(SAXBOSPIRAL).c $(SAXBOSPIRAL).h
	$(CC) $(CFLAGS) -o $(SAXBOSPIRAL)$(OS_NAME).o -c $(SAXBOSPIRAL).c

$(SOLVE)$(OS_NAME).o: $(SOLVE).c $(SAXBOSPIRAL).h
	$(CC) $(CFLAGS) -o $(SOLVE)$(OS_NAME).o -c $(SOLVE).c

$(SERIALISE)$(OS_NAME).o: $(SERIALISE).c $(SAXBOSPIRAL).h
	$(CC) $(CFLAGS) -o $(SERIALISE)$(OS_NAME).o -c $(SERIALISE).c

tests$(OS_NAME).o: $(SAXBOSPIRAL).h tests.c
	$(CC) $(CFLAGS) -o tests$(OS_NAME).o -c tests.c

$(TESTS): $(SAXBOSPIRAL)$(OS_NAME).o $(SOLVE)$(OS_NAME).o $(SERIALISE)$(OS_NAME).o tests$(OS_NAME).o
	$(CC) $(CFLAGS) -o tests$(OS_NAME)$(EXE_SUFFIX) $(SAXBOSPIRAL)$(OS_NAME).o $(SOLVE)$(OS_NAME).o $(SERIALISE)$(OS_NAME).o tests$(OS_NAME).o

prepare$(OS_NAME).o: $(SAXBOSPIRAL).h prepare.c
	$(CC) $(CFLAGS) -o prepare$(OS_NAME).o -c prepare.c

$(PREPARE): $(SAXBOSPIRAL)$(OS_NAME).o $(SERIALISE)$(OS_NAME).o prepare$(OS_NAME).o
	$(CC) $(CFLAGS) -o prepare$(OS_NAME)$(EXE_SUFFIX) $(SAXBOSPIRAL)$(OS_NAME).o $(SERIALISE)$(OS_NAME).o prepare$(OS_NAME).o

generate$(OS_NAME).o: $(SAXBOSPIRAL).h generate.c
	$(CC) $(CFLAGS) -o generate$(OS_NAME).o -c generate.c

$(GENERATE): $(SAXBOSPIRAL)$(OS_NAME).o $(SOLVE)$(OS_NAME).o $(SERIALISE)$(OS_NAME).o generate$(OS_NAME).o
	$(CC) $(CFLAGS) -o generate$(OS_NAME)$(EXE_SUFFIX) $(SAXBOSPIRAL)$(OS_NAME).o $(SOLVE)$(OS_NAME).o $(SERIALISE)$(OS_NAME).o generate$(OS_NAME).o

render$(OS_NAME).o: $(SAXBOSPIRAL).h render.c
	$(CC) $(CFLAGS) -o render$(OS_NAME).o -c render.c

$(RENDER): $(SAXBOSPIRAL)$(OS_NAME).o $(SERIALISE)$(OS_NAME).o render$(OS_NAME).o
	$(CC) $(CFLAGS) -o render$(OS_NAME)$(EXE_SUFFIX) $(SAXBOSPIRAL)$(OS_NAME).o $(SERIALISE)$(OS_NAME).o render$(OS_NAME).o $(LIBPNG)

test-unit: $(TESTS)
	./tests$(OS_NAME)$(EXE_SUFFIX)

test-func: $(PREPARE) $(GENERATE) $(RENDER)
	@echo "Running Functional Test"
	@echo -n "saxbospiral `git describe --abbrev=0`" > message.hex
	@./prepare$(OS_NAME)$(EXE_SUFFIX) message.hex message.sxp.hex
	@./generate$(OS_NAME)$(EXE_SUFFIX) message.sxp.hex
	@./render$(OS_NAME)$(EXE_SUFFIX) message.sxp.hex saxbospiral_test.png
	@diff saxbospiral.png saxbospiral_test.png
	@rm saxbospiral_test.png message.hex message.sxp.hex

logo: $(PREPARE) $(GENERATE) $(RENDER)
	@echo "Generating logo"
	@echo -n "saxbospiral `git describe --abbrev=0`" > saxbospiral.hex
	@./prepare$(OS_NAME)$(EXE_SUFFIX) saxbospiral.hex saxbospiral.sxp
	@./generate$(OS_NAME)$(EXE_SUFFIX) saxbospiral.sxp
	@./render$(OS_NAME)$(EXE_SUFFIX) saxbospiral.sxp saxbospiral.png
	@rm saxbospiral.hex saxbospiral.sxp

build: $(PREPARE) $(GENERATE) $(RENDER)

clean:
	rm -rf *.o *.out *.exe

all: test-unit test-func build
