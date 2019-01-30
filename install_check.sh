#!/bin/bash

wget https://github.com/libcheck/check/releases/download/0.12.0/check-0.12.0.tar.gz;
tar -xzvf check-0.12.0.tar.gz;
cd check-0.12.0;
./configure;
make -j;
make -j check;
sudo make install;
cd ..;
rm -rf check-0.12.0;
rm check-0.12.0.tar.gz;
