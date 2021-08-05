/**
 * @file
 *
 *
 **/
#pragma once

#include <iostream>
#include <regex>
#include <optional>

#include <fmt/chrono.h>
#include <fmt/format.h>
#include <fmt/compile.h>

#include <nlohmann/json.hpp>

#include <ghc/fs_std.hpp>

#include "spice_types.h"

namespace SugarSpice {
  
  /**
    * @brief ls, like in unix, kinda. Also it's a function.
    *
    * Iterates the input path and returning a list of files. Optionally, recursively. 
    *
    * @param root The root directory to search 
    * @param recursive recursively iterates through directories if true 
    * 
    * @returns list of paths
   **/
  std::vector<fs::path> ls(fs::path const & root, bool recursive);
  
  
  /**
    * @brief glob, like python's glob.glob, except C++
    *
    * Given a root and a regular expression, give all the files that match.  
    *
    * @param root The root directory to search 
    * @param reg std::regex object to pattern to search, defaults to ".*", or match averything.  
    * @param recursive recursively iterates through directories if true 
    * 
    * @returns list of paths matching regex
   **/
  std::vector<fs::path> glob(fs::path const & root, 
                             std::regex const & reg = std::regex(".*"), 
                             bool recursive=false);
  
  
  /**
    * @brief Get start and stop times a kernel. 
    *
    * For each segment in the kernel, get all start and stop times as a vector of double pairs. 
    * This gets all start and stop times regardless of the frame associated with it.
    * 
    * Input kernel is assumed to be a binary kernel with time dependant external orientation data. 
    * 
    * @param kpath Path to the kernel 
    * @returns std::vector of start and stop times  
   **/
  std::vector<std::pair<double, double>> getTimeIntervals(fs::path kpath);

  /**
   * @brief Simple struct for holding target states  
   */
  struct targetState {double lt; std::array<double,6> starg;};

  /**
   * @brief Gives the position and velocity for a given frame at some ephemeris time
   * 
   * Mostly a C++ wrap for NAIF's spkezr_c 
   * 
   * @param et ephemeris time at which you want to optain the target state 
   * @param target NAIF ID for the target frame 
   * @param observer NAIF ID for the observing frame 
   * @param frame The reference frame in which to get the positions in 
   * @param abcorr aborration correction flag, default it NONE. 
   *        This can set to:           
   *           "NONE" - No correction 
   *        For the "reception" case, i.e. photons from the target being recieved by the observer at the given time.           
   *           "LT"   - One way light time correction 
   *           "LT+S" - Correct for one-way light time and stellar aberration correction 
   *           "CN"   - Converging Newtonian light time correction
   *           "CN+S" - Converged Newtonian light time correction and stellar aberration correction  
   *        For the "transmission" case, i.e. photons emitted from the oberver hitting at target at the given time  
   *           "XLT"   - One-way light time correction using a newtonian formulation  
   *           "XLT+S" - One-way light time and stellar aberration correction using a newtonian formulation  
   *           "XCN"   - converged Newtonian light time correction
   *           "XCN+S" - converged Newtonian light time correction and stellar aberration correction.
   *  @return A TargetState struct with the light time adjustment and a Nx6 state vector of positions and velocities in x,y,z,vx,vy,vz format.   
  **/
  targetState getTargetState(double et, std::string target, std::string observer, std::string frame="J2000", std::string abcorr="NONE");


  /**
   * @brief simple struct for holding target orientations 
   */
  struct targetOrientation {std::array<double,4> quat; std::optional<std::array<double,3>> av;};


  /**
   * @brief Gives quaternion and angular velocity for a given frame at a given ephemeris time
   * 
   * Orientations for an input frame in some reference frame. 
   * The orientations returned from this function can be used to transform a position
   * in the source frame to the ref frame. 
   * 
   * @param et ephemeris time at which you want to optain the target pointing 
   * @param toframe the source frame's NAIF code. 
   * @param refframe the reference frame's NAIF code, orientations are relative to this reference frame
   * @returns SPICE-style quaternions (w,x,y,z) and optional angular velocity
  **/
  targetOrientation getTargetOrientation(double et, int toframe, int refframe=1); // use j2000 for default reference frame


  /**
    * @brief finds key:values in kernel pool
    *
    * Given a key template, returns matching key:values from the kernel pool
    *   by using gnpool, gcpool, gdpool, and gipool
    *
    * @param keytpl input key template to search for
    * 
    * @returns json list of found key:values
   **/
  nlohmann::json findKeywords(std::string keytpl);

  
  /**
    * @brief recursively search keys in json. 
    *
    * Given a root and a regular expression, give all the files that match.  
    *
    * @param in input json to search
    * @param key key to search for   
    * @param recursive recursively iterates through objects if true 
    * 
    * @returns vector of refernces to matching json objects 
   **/
  std::vector<nlohmann::json::json_pointer> findKeyInJson(nlohmann::json in, std::string key, bool recursive=true);
  

  /**
    * @brief This is a short description
    *
    * This is a long description 
    *
    * @param kpath The root directory to search 
    * @param sclk  Required sclk kernel
    * @param lsk   Required lsk kernel
    * 
    * @returns list of paths matching ext
   **/
  std::vector<std::pair<std::string, std::string>> getCkIntervals(std::string kpath, std::string sclk, std::string lsk);
  
  
  /** 
    * @brief get the Kernel type (CK, SPK, etc.) 
    * 
    *
    * @param kernelPath path to kernel 
    * @returns Kernel type as a string 
   **/
   std::string getKernelType(fs::path kernelPath);
  
  
  /** 
    * @brief Returns the path to the Mission specific Spice config file. 
    * 
    * Given a mission, search a prioritized list of directories for 
    * the json config file. This function checks in the order: 
    *  
    *   1. The local build dir, i.e. $CMAKE_SOURCE_DIR
    *   2. The install dir, i.e. $CMAKE_PREFIX 
    * 
    * @param mission mission name of the config file 
    * 
    * @returns path object of the condig file
   **/
   fs::path getMissionConfigFile(std::string mission);
  
  
  /** 
    * @brief Returns the path to the Mission specific Spice config file. 
    * 
    * Given a mission, search a prioritized list of directories for 
    * the json config file. This function checks in the order: 
    *  
    *   1. The local build dir, i.e. $CMAKE_SOURCE_DIR
    *   2. The install dir, i.e. $CMAKE_PREFIX 
    * 
    * @param mission mission name of the config file 
    * 
    * @returns path object of the condig file
   **/ 
   nlohmann::json getMissionConfig(std::string mission); 
  
  
  /** 
    * @brief Returns std::vector<string> interpretation of a json array. 
    * 
    * Attempts to convert the json array to a C++ array. Also handles 
    * strings in cases where one element arrays are stored as scalars. 
    * Throws exception if the json obj is not an array. 
    * 
    * @param arr input json arr
    * 
    * @returns string vector containing arr data
   **/ 
   std::vector<std::string> jsonArrayToVector(nlohmann::json arr);
  
  
  /** 
    * @brief Returns std::vector<string> interpretation of a json array. 
    * 
    * Attempts to convert the json array to a C++ array. Also handles 
    * strings in cases where one element arrays are stored as scalars. 
    * Throws exception if the json obj is not an array. 
    * 
    * @param arr input json arr
    * 
    * @returns string vector containing arr data
   **/ 
   fs::path getDataDirectory();
}