#!/usr/bin/env bash
echo -n "TEST" > test.hex;
./prepare test.hex test.sxp.hex;
time for i in {1..1000}; do ./generate test.sxp.hex; done;
