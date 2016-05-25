#!/usr/bin/env bash
echo -n $1 > temp.hex;
./prepare.out temp.hex temp.sxp.hex;
./generate.out temp.sxp.hex;
./render.out temp.sxp.hex $2;
rm temp.hex temp.sxp.hex;
