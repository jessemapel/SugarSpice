#pragma once 

/** 
  *
  *
 **/

#include <ghc/fs_std.hpp>
#include <iostream>

namespace SugarSpice {

  /**
    *
    *
    *
    *
    *
  **/
  class Kernel {
      public: 

      enum class Type { NA=0,
        CK, SPK, TSPK, 
        LSK, MK, SCLK,
        IAK, IK, FK, 
        DSK, PCK, EK
      }; 
      
      enum class Quality  {
        PREDICTED = 1,     // Based on predicted future location of the spacecraft/body     
        NADIR = 2,         // Assumes Nadir pointing   
        RECONSTRUCTED = 3, // Supplemented by real spacecraft/body data
        SMITHED = 4,       // Controlled Kernels
        NA = SMITHED       // Either Quaility doesn't apply (e.g. text kernels) -or- 
                            // we dont care about quality (e.g. CK of any quality)
      };

      const static std::vector<std::string> QUALITIES;
      const static std::vector<std::string> TYPES;

      static std::string translateType(Type type);
      static Type translateType(std::string type);

      static std::string translateQuality(Quality qa);
      static Quality translateQuality(std::string qa);

      static int translateFrame(std::string frame); 
      static std::string translateFrame(int frame); 

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

}
