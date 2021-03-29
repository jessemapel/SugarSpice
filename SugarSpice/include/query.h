/** 
  * @file 
  *
  *
  *
  *
  *
  *
 **/

 
 /**
   * @brief Instantiate a kernel from path 
   *
   * Load a kernel into memory by opening the kernel and furnishing 
   *
   * @param mission name of the mission. 
   * @param instrument name of the instrument. 
   * @param et time as Unix time   
   * 
  **/
 std::vector<std::string> searchCk(std::string mission, std::string instrument, double et);



 /**
   * @brief Instantiate a kernel from path 
   *
   * Load a kernel into memory by opening the kernel and furnishing 
   *
   * @param mission name of the mission. 
   * @param instrument name of the instrument. 
   * @param et time as Unix time   
   * 
  **/
 std::vector<std::string> searchSpk(std::string mission, std::string instrument, double et);