#!/usr/bin/env bash

# NOTE: This file assumes you are running linux and have installed cross-compilers for Windows and OSX.
# I used w64-mingw32-gcc and OSXCROSS (https://github.com/tpoechtrager/osxcross) respectively. 

$POSIX_EXTENSION='';
if [[ $USE_POSIX_EXTENSIONS ]]; then
    $POSIX_EXTENSION='.out';
fi

make OS_NAME=_linux_x86_64 EXE_SUFFIX=$POSIX_EXTENSION; # linux 64-bit target
make build tests OPTIMISE=-O3 CC=o64-clang OS_NAME=_macosx_x86_64 EXE_SUFFIX=$POSIX_EXTENSION; # osx 64-bit target
make prepare generate tests OPTIMISE=-O3 CC=x86_64-w64-mingw32-gcc OS_NAME=_windows_x86_64 EXE_SUFFIX=.exe; # windows 64-bit target
