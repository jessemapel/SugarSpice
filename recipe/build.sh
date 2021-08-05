#!/bin/sh
mkdir build && cd build
cmake .. -DCMAKE_INSTALL_PREFIX=$PREFIX -DSUGARSPICE_BUILD_DOCS=OFF 
make install
