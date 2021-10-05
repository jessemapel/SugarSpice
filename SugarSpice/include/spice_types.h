#pragma once

/**
  *
  *
 **/

#include <iostream>

namespace SugarSpice {

  /**
   * @brief Base Kernel class
   *
   * This is mostly designed to enable the automatic unloading
   * of kernels. The kernel is furnsh-ed on instantiation and
   * unloaded in the destructor.
   *
   */
  class Kernel {
      public:

      /**
       * @brief Enumeration representing the different possible kernel types
       **/
      enum class Type { NA=0,
        CK, SPK, TSPK,
        LSK, MK, SCLK,
        IAK, IK, FK,
        DSK, PCK, EK
      };

      /**
       * @brief Enumeration representing the different possible kernel qualities
       **/
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


      /**
       * @brief Switch between Kernel type enum to string
       *
       * @param type Kernel::Type to translate to a string
       * @return String representation of the kernel type, eg. Kernel::Type::CK returns "ck"
       **/
      static std::string translateType(Type type);


      /**
       * @brief Switch between Kernel type string to enum
       *
       * @param type String to translate to a Kernel::Type, must be all lower case
       * @return Kernel::Type representation of the kernel type, eg. "ck" returns Kernel::Type::CK
       **/
      static Type translateType(std::string type);


     /**
       * @brief Switch between Quality enum to string
       *
       * @param qa Kernel::Quality to translate to a string
       * @return String representation of the kernel type, eg. Kernel::Quality::Reconstructed  returns "reconstructed"
       **/
      static std::string translateQuality(Quality qa);


      /**
       * @brief Switch between Kernel quality string to enum
       *
       * @param qa String to translate to a Kernel::Quality, must be all lower case
       * @return Kernel::Type representation of the kernel type, eg. "reconstructed" returns Kernel::Quality::Reconstructed
       **/
      static Quality translateQuality(std::string qa);


      /**
       * @brief Switch between NAIF frame string code to integer frame code
       *
       * See <a href="https://naif.jpl.nasa.gov/pub/naif/toolkit_docs/C/req/naif_ids.html">NAIF's Docs on frame codes</a> for more information
       *
       * @param frame String to translate to a NAIF code
       * @return integer Naif frame code
       **/
      static int translateFrame(std::string frame);


      /**
       * @brief Switch between NAIF frame integer code to string frame code
       *
       * See <a href="https://naif.jpl.nasa.gov/pub/naif/toolkit_docs/C/req/naif_ids.html">NAIF's Docs on frame codes</a> for more information
       *
       * @param frame int NAIF frame code to translate
       * @return Translated string frame code
       **/
      static std::string translateFrame(int frame);


      /**
        * @brief Instantiate a kernel from path
        *
        * Load a kernel into memory by opening the kernel and furnishing
        *
        * @param path path to a kernel.
        *
      **/
      Kernel(std::string path);


      /**
        * @brief unfurnsh the kernel
        *
        * Delete a kernel from memory by deleting the object and unfurnshing.
        *
      **/
      ~Kernel();

      std::string path;   // path to the kernel
      Type type;       // type of kernel
      Quality quality; // quality of the kernel
  };


  /**
   * @brief typedef of std::shared_ptr<Kernel>
   *
   * This basically allows the Kernel to exist as a reference counted
   * variable. Once all references to the Kernel cease to exist, the kernel
   * is unloaded.
   */
  typedef std::shared_ptr<Kernel> SharedKernel;


  /**
   * @brief typedef of std::unique_ptr<Kernel>
   *
   * This basically allows the Kernel to exist only within the
   * call stack it is used in.
   */
  typedef std::unique_ptr<Kernel> StackKernel;


  /**
   * @brief convert a UTC string to an ephemeris time
   *
   * Basically a wrapper around NAIF's cspice str2et function except it also temporarily loads the required kernels.
   * See Also: https://naif.jpl.nasa.gov/pub/naif/toolkit_docs/C/cspice/str2et_c.html
   *
   * @param et UTC string, e.g. "1988 June 13, 12:29:48 TDB"
   * @returns double precision ephemeris time
   **/
  double utcToEt(std::string et);
}
