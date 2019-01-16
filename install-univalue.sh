#!/bin/sh
set -ex
wget https://github.com/jgarzik/univalue/archive/v1.0.4.tar.gz
tar -xzvf v1.0.4.tar.gz
cd univalue-1.0.4 && ./configure --prefix=/usr && make && sudo make install
