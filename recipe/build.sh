#!/bin/sh
mkdir build && cd build
cmake .. -DCMAKE_INSTALL_PREFIX=$PREFIX -DSPICEQL_BUILD_DOCS=OFF 
make install
