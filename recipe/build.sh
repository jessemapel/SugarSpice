#!/bin/sh
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=RELEASE -DCMAKE_INSTALL_PREFIX=$PREFIX -DSPICEQL_BUILD_DOCS=OFF -DSPICEQL_BUILD_TESTS=OFF ..
cmake --build . --config RELEASE
cmake --install . --config RELEASE
cmake --build . --target install-python
