#!/bin/bash
# 
# Functional test script.
# Generates a new logo and compares with the current one.
# The paths of the prepare, generate and render commands are
# passed as the first 3 arguments.
# 
./build_logo.sh "$1" "$2" "$3" "test_saxbospiral.png" && \
diff "saxbospiral.png" "test_saxbospiral.png" && \
rm "test_saxbospiral.png";
