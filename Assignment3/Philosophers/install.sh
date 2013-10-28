#!/bin/bash

# Install ZeroMQ
sudo apt-get install libtool autoconf automake uuid-dev build-essential
cd /tmp
wget http://download.zeromq.org/zeromq-4.0.1.tar.gz
tar zxvf zeromq-4.0.1.tar.gz && cd zeromq-4.0.1
./configure
make
sudo make install
sudo ldconfig

# Install C bindings
cd /tmp
wget http://download.zeromq.org/czmq-2.0.2.tar.gz
tar xzf czmq-2.0.2.tar.gz
cd czmq-2.0.2
./configure
make
sudo make install
sudo ldconfig

# Install Python bindings
sudo apt-get install python-dev python-setuptools
sudo easy_install-2.7 pip
sudo pip-2.7 install pyzmq
