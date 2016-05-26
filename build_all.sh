#!/usr/bin/env bash

# NOTE: This file assumes you are running linux and have installed cross-compilers for Windows and OSX.
# I used w64-mingw32-gcc and OSXCROSS (https://github.com/tpoechtrager/osxcross) respectively. 

OPTIMISE="-O3";

# build and run everything for linux
make OPTIMISE=$OPTIMISE OS_NAME=_linux_x86_64; # linux 64-bit target
# clean up generated object code files for next run
make clean-objects;
# build everything but don't run anything for osx
make build tests OPTIMISE=$OPTIMISE CC=o64-clang OS_NAME=_macosx_x86_64; # osx 64-bit target
# clean up generated object code files for next run
make clean-objects;
# build everything except render and run the unit tests for windows
make prepare generate tests test-unit OPTIMISE=$OPTIMISE CC=x86_64-w64-mingw32-gcc OS_NAME=_windows_x86_64 EXE_SUFFIX=.exe; # windows 64-bit target
# clean up generated object code files for next run
make clean-objects;
