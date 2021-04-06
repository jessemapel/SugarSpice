#pragma once 

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


 /**
  * @brief Returns all kernels available for a mission
  *
  * Returns a structured json object containing all available kernels for a specified mission 
  * along with their dependencies.
  *  
  * TODO: Add a "See Also" on json format after the format matures a bit more. 
  *
  * @param
  * @returns list of paths matching ext
  **/
 nlohmann::json searchMissionKernels(fs::path root, std::string mission);