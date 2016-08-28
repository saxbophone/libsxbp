#!/bin/bash
# 
# Functional test script.
# Generates a new logo and compares with the current one.
# The paths of the prepare, generate and render commands are
# passed as the first 3 arguments. The 4th argument is the
# message to use for the file (see build_logo.sh)
# 
./build_logo.sh "$1" "$2" "$3" "test_saxbospiral.png" "$4" && \
diff "saxbospiral.png" "test_saxbospiral.png" && \
rm "test_saxbospiral.png";
