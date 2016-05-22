#!/usr/bin/env bash

# NOTE: This file assumes you are running linux and have installed cross-compilers for Windows and OSX.
# I used w64-mingw32-gcc and OSXCROSS (https://github.com/tpoechtrager/osxcross) respectively. 

$POSIX_EXTENSION='';
if [[ $USE_POSIX_EXTENSIONS ]]; then
    $POSIX_EXTENSION='.out';
fi

make prepare generate tests OS_NAME=_lin64 EXE_SUFFIX=$POSIX_EXTENSION; # linux 64-bit target
make prepare generate tests OPTIMISE=-O3 CC=o32-clang OS_NAME=_osx32 EXE_SUFFIX=$POSIX_EXTENSION; # osx 32-bit target
make prepare generate tests OPTIMISE=-O3 CC=o64-clang OS_NAME=_osx64 EXE_SUFFIX=$POSIX_EXTENSION; # osx 64-bit target
make prepare generate tests OPTIMISE=-O3 CC=i686-w64-mingw32-gcc OS_NAME=_win32 EXE_SUFFIX=.exe; # windows 32-bit target
make prepare generate tests OPTIMISE=-O3 CC=x86_64-w64-mingw32-gcc OS_NAME=_win64 EXE_SUFFIX=.exe; # windows 64-bit target
