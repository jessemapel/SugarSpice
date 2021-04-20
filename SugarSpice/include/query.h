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
 nlohmann::json searchMissionKernels(fs::path root,  nlohmann::json conf);