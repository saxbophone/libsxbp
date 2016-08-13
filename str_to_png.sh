#!/usr/bin/env bash
echo -n $1 > temp.hex;
./prepare temp.hex temp.sxp.hex;
./generate temp.sxp.hex;
./renderer temp.sxp.hex $2;
rm temp.hex temp.sxp.hex;
