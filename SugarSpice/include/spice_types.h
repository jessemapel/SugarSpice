#pragma once

/**
  *
  *
 **/

#include <iostream>

/**
 * @namespace SugarSpice types
 * 
 */
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

      /**
       * used for converting string and int kernal quality
       */
      const static std::vector<std::string> QUALITIES;
      
      /**
       * used for converting between string and int kernal types
       */
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
        * Load a kernel into memory by opening the kernel and furnishing.
        * This also increases the reference count of the kernel. If the kernel
        * has alrady been furnished, it is refurnshed.  
        *
        * @param path path to a kernel.
        *
      **/
      Kernel(std::string path);


      /**
        * @brief Delete the kernel object and decrease it's reference count
        *
        * Deletes the kernel object and decrements it's reference count. If the reference count hits 0,
        * the kernel is unloaded. 
        *
      **/
      ~Kernel();

      /*! path to the kernel */
      std::string path; 
      /*! type of kernel */
      Type type; 
      /*! quality of the kernel */
      Quality quality;
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
   *
   */
  typedef std::unique_ptr<Kernel> StackKernel;

  class KernelPool {
    public:
    
    /**
     * Delete constructors and such as this is a static class
     */
    KernelPool(KernelPool &other) = delete;
    KernelPool() = delete;
    ~KernelPool() = delete;


    /**
     * @brief get a kernel's reference count
     * 
     * Everytime KernelPool::load is called, the reference count is increased by one. 
     * This returns the number of Kernel objects currently are currently referencing the 
     * input kernel.  
     *
     * @param key key for the kernel to get the ref count for, usually the complete file path
     * @return unsigned int The number of references to the input kernel. If key doesn't exist, this is 0. 
     */
    unsigned int refCount(std::string key);


    /**
     * @brief get reference counts for all kernels in the pool 
     * 
     * Everytime KernelPool::load is called, the reference count is increased by one. 
     * This returns the number of Kernel objects referencing every Kernel in the pool.
     *
     * @return std::map<std::string, int> Map of kernel path to reference count.
     */
    static std::map<std::string, int> refCounts();


    /**
     * @brief load kernel into the kernel pool 
     * 
     * This should be called for furnshing kernel instead of furnsh_c directly 
     * so that they are tracked throughout the process. 
     *
     *
     * @param kernel Path to the kernel to load 
     * @param force_furnsh If true, call furnsh on the kernel even if the kernel is already in the pool. Default is False. 
     */
    static int load(std::string kernelPath, bool force_refurnsh=false);


    /**
     * @brief reduce the reference count for a kernel 
     * 
     * This reduces the ref count by one, and if the ref count hits 0, 
     * the kernel is unfurnished. Use this instead of calling unload_c 
     * directly as you cause errors. 
     * 
     * @param kernelPath path to the kernel
     */
    static int unload(std::string kernelPath);

    private: 

    /**
     * @brief 
     * 
     */
    static std::unordered_map<std::string, int> refCounts; 
    
 
  };

  class KernelSet {
    public:
    KernelSet(nlohmann::json kernels);
    ~KernelSet();

    std::unordered_map<std::string, std::vector<Kernel>> loadedKernels;
    nlohmann::json kernels; 
  };


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
