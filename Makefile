.DEFAULT_GOAL := build

CC=gcc
CFLAGS=-std=c99
LIBRARY=saxbospiral.c
TESTS=tests.c
COMMANDS=prepare.c generate.c
SILENT=@

compile-library:
	$(SILENT)$(CC) $(CFLAGS) -c $(LIBRARY)

compile-tests:
	$(SILENT)$(CC) $(CFLAGS) -c $(TESTS)

tests: compile-library compile-tests
	$(SILENT)$(CC) -g -o test saxbospiral.o tests.o

compile-commands:
	$(SILENT)$(CC) $(CFLAGS) -c $(COMMANDS)

commands: compile-library compile-commands
	$(SILENT)$(CC) -g -o prepare saxbospiral.o prepare.o
	$(SILENT)$(CC) -g -o generate saxbospiral.o generate.o

test: tests
	$(SILENT)./test

clean:
	$(SILENT)rm *.o

build: test commands
