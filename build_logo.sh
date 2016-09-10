#!/bin/bash
# 
# Script for updating the project logo
# The first argument is the path to the sxp cli program.
# The second argument is the file to write the PNG output to.
# The third is the message to use for the logo data
# (of format "saxbospiral vX.Y.Z").
# 
echo "Generating logo";
echo -n "$3" > temp.hex && \
./"$1" -pgr -i temp.hex -o "$2" && \
rm temp.hex;
