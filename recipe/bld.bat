mkdir build
cd build
cmake .. -DCMAKE_INSTALL_PREFIX=$PREFIX -DSUGARSPICE_BUILD_DOCS=OFF 
make install
if errorlevel 1 exit 1
