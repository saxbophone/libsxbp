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
RENDER_BACKENDs=render_backends/

SAXBOSPIRAL=$(LIB)saxbospiral
INITIALISE=$(LIB)initialise
PLOT=$(LIB)plot
RENDER=$(LIB)render
SOLVE=$(LIB)solve
SERIALISE=$(LIB)serialise
PNG_BACKEND=$(LIB)$(RENDER_BACKENDs)png_backend

$(SAXBOSPIRAL).o: $(SAXBOSPIRAL).c $(SAXBOSPIRAL).h
	$(CC) $(CFLAGS) -o $(SAXBOSPIRAL).o -c $(SAXBOSPIRAL).c

$(INITIALISE).o: $(INITIALISE).c $(INITIALISE).h $(SAXBOSPIRAL).h
	$(CC) $(CFLAGS) -o $(INITIALISE).o -c $(INITIALISE).c

$(PLOT).o: $(PLOT).c $(PLOT).h $(SAXBOSPIRAL).h
	$(CC) $(CFLAGS) -o $(PLOT).o -c $(PLOT).c

$(RENDER).o: $(RENDER).c $(RENDER).h $(SAXBOSPIRAL).h
	$(CC) $(CFLAGS) -o $(RENDER).o -c $(RENDER).c

$(SOLVE).o: $(SOLVE).c $(SAXBOSPIRAL).h
	$(CC) $(CFLAGS) -o $(SOLVE).o -c $(SOLVE).c

$(SERIALISE).o: $(SERIALISE).c $(SAXBOSPIRAL).h
	$(CC) $(CFLAGS) -o $(SERIALISE).o -c $(SERIALISE).c

$(PNG_BACKEND).o: $(PNG_BACKEND).c $(RENDER).h
	$(CC) $(CFLAGS) -o $(PNG_BACKEND).o -c $(PNG_BACKEND).c

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

renderer.o: $(SAXBOSPIRAL).h $(PLOT).h renderer.c
	$(CC) $(CFLAGS) -o renderer.o -c renderer.c

renderer: $(SAXBOSPIRAL).o $(PLOT).o $(PNG_BACKEND).o $(RENDER).o $(SERIALISE).o renderer.o
	$(CC) $(CFLAGS) -o renderer$(OS_NAME)$(EXE_SUFFIX) $(SAXBOSPIRAL).o $(PNG_BACKEND).o $(PLOT).o $(RENDER).o $(SERIALISE).o renderer.o $(LIBPNG)

test-unit: tests
	./tests$(OS_NAME)$(EXE_SUFFIX)

test-func: prepare generate renderer
	@echo "Running Functional Test"
	@echo -n "saxbospiral `git describe --abbrev=0`" > message.hex
	@./prepare$(OS_NAME)$(EXE_SUFFIX) message.hex message.sxp.hex
	@./generate$(OS_NAME)$(EXE_SUFFIX) message.sxp.hex
	@./renderer$(OS_NAME)$(EXE_SUFFIX) message.sxp.hex saxbospiral_test.png
	@diff saxbospiral.png saxbospiral_test.png
	@rm saxbospiral_test.png message.hex message.sxp.hex

logo: prepare generate renderer
	@echo "Generating logo"
	@echo -n "saxbospiral `git describe --abbrev=0`" > saxbospiral.hex
	@./prepare$(OS_NAME)$(EXE_SUFFIX) saxbospiral.hex saxbospiral.sxp
	@./generate$(OS_NAME)$(EXE_SUFFIX) saxbospiral.sxp
	@./renderer$(OS_NAME)$(EXE_SUFFIX) saxbospiral.sxp saxbospiral.png
	@rm saxbospiral.hex saxbospiral.sxp

build: prepare generate renderer

clean-objects:
	rm -rf *.o saxbospiral/*.o saxbospiral/render_backends/*.o

clean-executables:
	rm -rf *.out *.exe *.x86_64 tests prepare generate renderer

clean: clean-objects clean-executables

all: test-unit test-func build
