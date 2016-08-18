#!/bin/bash
# 
# Script for updating the project logo
# The paths of the prepare, generate and render commands are
# passed as the first 3 arguments. The last argument is the
# file to write the PNG output to.
# 
echo "Generating logo";
echo -n "saxbospiral `git describe --abbrev=0`" > temp.hex && \
./"$1" temp.hex temp.sxp && \
./"$2" temp.sxp && \
./"$3" temp.sxp "$4" && \
rm temp.hex temp.sxp;
