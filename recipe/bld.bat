mkdir build
cd build
cmake .. -DCMAKE_INSTALL_PREFIX=$PREFIX
make install
if errorlevel 1 exit 1
