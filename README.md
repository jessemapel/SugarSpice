# SugarSpice
[![Documentation Status](https://readthedocs.org/projects/sugar-spice/badge/?version=latest)](http://sugar-spice.readthedocs.io/?badge=latest) [![CMake](https://github.com/USGS-Astrogeology/SugarSpice/actions/workflows/ctests.yml/badge.svg)](https://github.com/USGS-Astrogeology/SugarSpice/actions/workflows/ctests.yml)

This Library provides a C++ interface querying, reading and writing Naif SPICE kernels. Built on the [Naif Toolkit](https://naif.jpl.nasa.gov/naif/toolkit.html).

## Roadmap

This is still a work in progress. Developed by USGS Astrogeology, this library is intended to be used as part of Astrogeology's Software portfolio. Specifically as [Ale's](https://github.com/USGS-Astrogeology/Ale) primary method for interacting with kernels.

Library name will most likely change as this matures.

Primary Milstones:

 1. Complete basic I/O functionality. This Includes:
    * Complete Query support for at least one mission
    * Complete Kernel writing for CK, SPK and Text kernels from C++ types
 2. Complete Mission Support
    * Complate Configuration files for all missions currently supported by ISIS
 3. Complete reading of Binary Spice kernels to C++ types

## Building The Library

The library leverages anaconda to maintain all of it's dependencies. So in order to build SugarSpice, you'll need to have Anaconda installed.

> **NOTE**:If you already have Anaconda installed, skip to step 3.

1. Download either the Anaconda or Miniconda installation script for your OS platform. Anaconda is a much larger distribtion of packages supporting scientific python, while Miniconda is a minimal installation and not as large: Anaconda installer, Miniconda installer
1. If you are running on some variant of Linux, open a terminal window in the directory where you downloaded the script, and run the following commands. In this example, we chose to do a full install of Anaconda, and our OS is Linux-based. Your file name may be different depending on your environment.
   * If you are running Mac OS X, a pkg file (which looks similar to Anaconda3-5.3.0-MacOSX-x86_64.pkg) will be downloaded. Double-click on the file to start the installation process.
1. Open a Command line prompt and run the following commands:

```bash
# Clone the Github repo, note the recursive flag, this library depends on
# submodules that also need to be cloned. --recurse-submodules enables this and
# the -j8 flag parallelizes the cloning process.
git clone --recurse-submodules -j8 https://github.com/USGS-Astrogeology/SugarSpice.git

# cd into repo dir
cd SugarSpice

# Create new environment from the provided dependency file, the -n flag is
# proceded by the name of the new environment, change this to whatever works for you
conda env create -f environment.yml -n ssdev

# activate the new env
conda activate ssdev

# make and cd into the build directory. This can be placed anywhere, but here, we make
# it in the repo (build is in .gitingore, so no issues there)
mkdir build
cd build

# Configure the project, install directory can be anything, here, it's the conda env
cmake .. -DCMAKE_INSTALL_PREFIX=$CONDA_PREFIX

# Optional: DB files are installed by default in $CONDA_PREFIX/etc/SugarSpice/db to 
# use files that are included within the repo, you must create and define 
# an environment variable named SSPICE_DEBUG. 
# note SSPICE_DEBUG can be set to anything as long as it is defined
export SSPICE_DEBUG=True

# Set the environment variable(s) to point to your kernel install 
# The following environment variables are used by default in order of priority: 
# $SPICEROOT, $ALESPICEROOT, $ISISDATA. These variabels are used by both ALE and 
# ISIS respectively. 
# note you can set each of these environment variables path to point to the
# correspoding kernels downloaded location, ie 
SPICEROOT=~/sugarSpice/Kernals/spiceRootKernel
ALESPICEROOT=~/sugarSpice/Kernals/aleSpiceRootKernel
ISISDATA=~/sugarSpice/Kernals/isisData

# build and install project
make install

# Optional, Run tests
ctest -j8
```

You can disable different components of the build by setting the CMAKE variables `SUGARSPICE_BUILD_DOCS`, `SUGARSPICE_BUILD_TESTS`, or `SUGARSPICE_BUILD_LIB` to `OFF`. For example, the following cmake configuration command will not build the documentation or the tests:

```
cmake .. -DCMAKE_INSTALL_PREFIX=$CONDA_PREFIX -DSUGARSPICE_BUILD_DOCS=OFF -DSUGARSPICE_BUILD_TESTS=OFF
```
