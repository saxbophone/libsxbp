.DEFAULT_GOAL := all
.PHONY: test-unit test-func logo build clean-objects clean-executables clean all

CC=gcc
STANDARD=-std=c99
OPTIMISE=-O0
DEBUG=-g
INCLUDES=
CFLAGS=$(STANDARD) $(OPTIMISE) $(DEBUG) $(INCLUDES)
LIBPNG=-lpng
OS_NAME=
EXE_SUFFIX=

LIB=saxbospiral/

SAXBOSPIRAL=$(LIB)saxbospiral
INITIALISE=$(LIB)initialise
PLOT=$(LIB)plot
SOLVE=$(LIB)solve
SERIALISE=$(LIB)serialise

$(SAXBOSPIRAL).o: $(SAXBOSPIRAL).c $(SAXBOSPIRAL).h
	$(CC) $(CFLAGS) -o $(SAXBOSPIRAL).o -c $(SAXBOSPIRAL).c

$(INITIALISE).o: $(INITIALISE).c $(INITIALISE).h $(SAXBOSPIRAL).h
	$(CC) $(CFLAGS) -o $(INITIALISE).o -c $(INITIALISE).c

$(PLOT).o: $(PLOT).c $(PLOT).h $(SAXBOSPIRAL).h
	$(CC) $(CFLAGS) -o $(PLOT).o -c $(PLOT).c

$(SOLVE).o: $(SOLVE).c $(SAXBOSPIRAL).h
	$(CC) $(CFLAGS) -o $(SOLVE).o -c $(SOLVE).c

$(SERIALISE).o: $(SERIALISE).c $(SAXBOSPIRAL).h
	$(CC) $(CFLAGS) -o $(SERIALISE).o -c $(SERIALISE).c

tests.o: $(SAXBOSPIRAL).h tests.c
	$(CC) $(CFLAGS) -o tests.o -c tests.c

tests: $(SAXBOSPIRAL).o $(INITIALISE).o $(PLOT).o $(SOLVE).o $(SERIALISE).o tests.o
	$(CC) $(CFLAGS) -o tests$(OS_NAME)$(EXE_SUFFIX) $(SAXBOSPIRAL).o $(INITIALISE).o $(PLOT).o $(SOLVE).o $(SERIALISE).o tests.o

prepare.o: $(SAXBOSPIRAL).h prepare.c
	$(CC) $(CFLAGS) -o prepare.o -c prepare.c

prepare: $(SAXBOSPIRAL).o $(INITIALISE).o $(SERIALISE).o prepare.o
	$(CC) $(CFLAGS) -o prepare$(OS_NAME)$(EXE_SUFFIX) $(SAXBOSPIRAL).o $(INITIALISE).o $(SERIALISE).o prepare.o

generate.o: $(SAXBOSPIRAL).h $(PLOT).h generate.c
	$(CC) $(CFLAGS) -o generate.o -c generate.c

generate: $(SAXBOSPIRAL).o $(PLOT).o $(SOLVE).o $(SERIALISE).o generate.o
	$(CC) $(CFLAGS) -o generate$(OS_NAME)$(EXE_SUFFIX) $(SAXBOSPIRAL).o $(PLOT).o $(SOLVE).o $(SERIALISE).o generate.o

render.o: $(SAXBOSPIRAL).h $(PLOT).h render.c
	$(CC) $(CFLAGS) -o render.o -c render.c

render: $(SAXBOSPIRAL).o $(PLOT).o $(SERIALISE).o render.o
	$(CC) $(CFLAGS) -o render$(OS_NAME)$(EXE_SUFFIX) $(SAXBOSPIRAL).o $(PLOT).o $(SERIALISE).o render.o $(LIBPNG)

test-unit: tests
	./tests$(OS_NAME)$(EXE_SUFFIX)

test-func: prepare generate render
	@echo "Running Functional Test"
	@echo -n "saxbospiral `git describe --abbrev=0`" > message.hex
	@./prepare$(OS_NAME)$(EXE_SUFFIX) message.hex message.sxp.hex
	@./generate$(OS_NAME)$(EXE_SUFFIX) message.sxp.hex
	@./render$(OS_NAME)$(EXE_SUFFIX) message.sxp.hex saxbospiral_test.png
	@diff saxbospiral.png saxbospiral_test.png
	@rm saxbospiral_test.png message.hex message.sxp.hex

logo: prepare generate render
	@echo "Generating logo"
	@echo -n "saxbospiral `git describe --abbrev=0`" > saxbospiral.hex
	@./prepare$(OS_NAME)$(EXE_SUFFIX) saxbospiral.hex saxbospiral.sxp
	@./generate$(OS_NAME)$(EXE_SUFFIX) saxbospiral.sxp
	@./render$(OS_NAME)$(EXE_SUFFIX) saxbospiral.sxp saxbospiral.png
	@rm saxbospiral.hex saxbospiral.sxp

build: prepare generate render

clean-objects:
	rm -rf *.o saxbospiral/*.o

clean-executables:
	rm -rf *.out *.exe *.x86_64 tests prepare generate render

clean: clean-objects clean-executables

all: test-unit test-func build
