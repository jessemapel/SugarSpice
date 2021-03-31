/**
 * @file
 *
 *
 **/

#include <iostream>
#include <regex>

#include <fmt/chrono.h>
#include <fmt/format.h>
#include <fmt/compile.h>

#include <ghc/fs_std.hpp>

#include "spice_types.h"

/**
  * @brief This is a short description
  *
  * This is a long description 
  *
  * @param root The root directory to search 
  * @param ext  File Extension 
  * 
  * @returns list of paths matching ext
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
  * @brief This is a short description
  *
  * This is a long description 
  *
  * 
  * @returns list of paths matching ext
 **/
fs::path getKernelDir(fs::path root, std::string mission, std::string instrument, Kernel::Type type);


/** 
  *
  *
  *
  *
 **/
 std::string getKernelType(fs::path kernelPath);


/** 
  *
  *
  *
  *
 **/
 int translateFrame(std::string frame); 


/** 
  *
  *
  *
  *
 **/
 std::string translateFrame(int frame); 


/** 
  *
  *
  *
  *
 **/
 fs::path getDbFile(std::string mission);