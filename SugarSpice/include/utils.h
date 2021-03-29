/**
 *
 *
 *
 **/

#include <iostream>

#include <fmt/chrono.h>
#include <fmt/format.h>
#include <fmt/compile.h>

#include <ghc/fs_std.hpp>
#include <SpiceUsr.h>

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
                           std::string const & ext);


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
std::vector<std::pair<std::string, std::string>> FormatIntervals(SpiceCell &coverage, std::string type,
                                     double startOffset, double endOffset);


/**
  * @brief Format interavals into a 
  *
  * This is a long description 
  *
  * @param coverage a SpiceCell holding coverage values. 
  * @param ext  File Extension 
  * 
  * @returns list of paths matching ext
 **/
std::vector<std::pair<std::string, std::string>> FormatFirstLastIntervals(SpiceCell &coverage, std::string type,
                                     double startOffset, double endOffset);


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


