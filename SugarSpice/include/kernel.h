/**
  *
  *
  *  
  *
  *
  *
 **/


class kernel {
    public: 

    /**
      * @brief Instantiate a kernel from path 
      *
      * Load a kernel into memory by opening the kernel and furnishing 
      *
      * @param path path to a kernel. 
      * 
     **/
    kernel(std::string path);
    

    /**
      * @brief unfurnsh the kernel 
      *
      * Delete a kernel from memory by deleting the object and unfurnshing. 
      * 
     **/
    ~kernel();    

}