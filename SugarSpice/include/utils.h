/**
 * @file
 *
 *
 **/
#pragma once

#include <iostream>
#include <regex>

#include <fmt/chrono.h>
#include <fmt/format.h>
#include <fmt/compile.h>

#include <nlohmann/json.hpp>

#include <ghc/fs_std.hpp>

#include "spice_types.h"


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
  *
  *
  *
  *
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