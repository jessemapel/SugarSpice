/**
  * 
  *
  *
  *
 **/


#include <SpiceUsr.h>
#include <ghc/fs_std.hpp>

#include "kernel.h"

using namespace std; 

Kernel::Kernel(fs::path path) { 
    this->path = path; 
    furnsh_c(path.c_str());
}


Kernel::~Kernel() {
    unload_c(this->path.c_str());
}