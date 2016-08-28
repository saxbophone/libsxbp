#!/bin/bash
# 
# Script for updating the project logo
# The paths of the prepare, generate and render commands are
# passed as the first 3 arguments. The 4th argument is the
# file to write the PNG output to and the 5th is the message
# to use for the logo data (of format "saxbospiral vX.Y.Z")
# 
echo "Generating logo";
echo -n "$5" > temp.hex && \
./"$1" temp.hex temp.sxp && \
./"$2" temp.sxp && \
./"$3" temp.sxp "$4" && \
rm temp.hex temp.sxp;
