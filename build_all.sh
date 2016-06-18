#!/usr/bin/env bash

# NOTE: This file assumes you are running linux and have installed cross-compilers for Windows and OSX.
# I used w64-mingw32-gcc and OSXCROSS (https://github.com/tpoechtrager/osxcross) respectively. 

OPTIMISE="-O3";

green=`tput setaf 2`
dim=`tput dim`
reset=`tput sgr0`

echo "${reset}${green}Cleaning Up${reset}${dim}";
make clean-objects;
# build and run everything for linux
echo "${reset}Building Linux${dim}";
make OPTIMISE=$OPTIMISE OS_NAME=_linux EXE_SUFFIX=.x86_64; # linux 64-bit target
# clean up generated object code files for next run
echo "${reset}${green}Cleaning Up${reset}${dim}";
make clean-objects;
# build everything but don't run anything for osx
echo "${reset}Building Mac OSX${dim}";
make build tests OPTIMISE=$OPTIMISE CC=o64-clang OS_NAME=_macosx EXE_SUFFIX=.x86_64; # osx 64-bit target
# clean up generated object code files for next run
echo "${reset}${green}Cleaning Up${reset}${dim}";
make clean-objects;
# build everything except render and run the unit tests for windows
echo "${reset}Building Microsoft Windows${dim}";
make prepare generate tests test-unit OPTIMISE=$OPTIMISE CC=x86_64-w64-mingw32-gcc OS_NAME=_windows_x86_64 EXE_SUFFIX=.exe; # windows 64-bit target
# clean up generated object code files for next run
echo "${reset}${green}Cleaning Up${reset}${dim}";
make clean-objects;
echo -n "${reset}";
