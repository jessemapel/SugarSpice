/**
  *
  *
  *  
  *
  *
  *
 **/

#include <iostream>
#include <ghc/fs_std.hpp>


class Kernel {
    public: 

    /**
      * @brief Instantiate a kernel from path 
      *
      * Load a kernel into memory by opening the kernel and furnishing 
      *
      * @param path path to a kernel. 
      * 
     **/
    Kernel(fs::path path);
    

    /**
      * @brief unfurnsh the kernel 
      *
      * Delete a kernel from memory by deleting the object and unfurnshing. 
      * 
     **/
    ~Kernel();    

    fs::path path;
};