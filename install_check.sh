#!/bin/bash

wget https://github.com/libcheck/check/releases/download/0.12.0/check-0.12.0.tar.gz
tar -xzvf check-0.12.0.tar.gz
cd check-0.12.0
cmake .
make -j
sudo make install
