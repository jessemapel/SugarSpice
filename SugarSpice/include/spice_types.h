/** 
  *
  *
 **/

#include <ghc/fs_std.hpp>
#include <iostream>


/**
  *
  *
  *
  *
  *
 **/
class Kernel {
    public: 

    enum Type {
      CK=0, SPK, TSPK, 
      LSK, MK, SCLK,
      IAK, IK, FK, 
      DSK, PCK, EK
    }; 
    
    enum Quality  { 
        NA = 0,        // Non External Orientation data
        Predicted,     // Based on predicted location of instrument     
        Nadir,         // Assumes Nadir pointing   
        Reconstructed, // 
        Smithed        // Controlled Kernels
    };

    const static std::vector<std::string> QUALITIES;
    const static std::vector<std::string> TYPES;

    static std::string translateType(Type type);
    static Type translateType(std::string type);

    static std::string translateQuality(Quality qa);
    static Quality translateQuality(std::string qa);

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
    Type type; 
    Quality quality; 
};