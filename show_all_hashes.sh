#!/usr/bin/env bash
for HASH in `git rev-list HEAD`;
do ./str_to_png.sh "`echo -n $HASH | xxd -r -p -s -+-20`" $HASH.png;
done;
