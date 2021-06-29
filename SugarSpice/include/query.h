#pragma once 

#include <vector>
#include <iostream>
#include <nlohmann/json.hpp>
#include <ghc/fs_std.hpp>


/** 
  * @file 
  * 
  * Functions that interigate the user's kernel installation to return kernels lists 
  * or SPICE information are located here
  *
 **/


fs::path getLatestKernel(std::vector<fs::path> kernels);


/**
 * @brief Returns all kernels available for a mission
 *
 * Returns a structured json object containing all available kernels for a specified mission 
 * along with their dependencies.
 *  
 * TODO: Add a "See Also" on json format after the format matures a bit more. 
 *
 * @param root root path to search 
 * @param conf json conf file 
 * @returns list of paths matching ext
**/
nlohmann::json searchMissionKernels(fs::path root,  nlohmann::json conf);


/**
 * @brief Returns all kernels available in the time range 
 *
 * Returns a structured json object containing all available kernels for a specified time  
 * range along with their dependencies.
 *  
 * TODO: Add a "See Also" on json format after the format matures a bit more. 
 *
 * @param kernels kernels to search 
 * @param times vector of times to match 
 * @param isContiguous if true, all times need to be in the kernel to match the query, else, any kernel that 
                       is in any of the times inputed get returned 
 * @returns json object with new kernels 
**/
nlohmann::json searchMissionKernels(nlohmann::json kernels, std::vector<double> times, bool isContiguous=false);


/**
  * Lambda for parsing a CK json object, returns a json object
  * with a similar structure, but regexes replaces with a path list
 **/
nlohmann::json globKernels(fs::path root, nlohmann::json conf, std::string kernelType);