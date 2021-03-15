#include <iostream>

#include <fmt/chrono.h>
#include <fmt/format.h>
#include <fmt/compile.h>

#include <ghc/fs_std.hpp>
#include <SpiceUsr.h>

namespace chrono = std::chrono;

std::string calForm = "YYYY MON DD HR:MN:SC.###### TDB ::TDB";

std::vector<fs::path> glob(fs::path const & root, std::string const & ext) {
    std::vector<fs::path> paths;

    if (fs::exists(root) && fs::is_directory(root)) {
        for (auto const & entry : fs::recursive_directory_iterator(root)) {
            if (fs::is_regular_file(entry) && entry.path().extension() == ext)
                paths.emplace_back(entry.path().filename());
        }
    }

    return paths;
}             


std::vector<std::pair<std::string, std::string>> FormatIntervals(SpiceCell &coverage, std::string type,
                                     double startOffset, double endOffset) {
  SpiceChar begStr[35], endStr[35];
  //Get the number of intervals in the object.
  int niv = card_c(&coverage) / 2;
  //Convert the coverage interval start and stop times to TDB
  double begin, end;
  
  std::vector<std::pair<std::string, std::string>> results;
  // std::cout << niv << std::endl; 
  for(int j = 0;  j < niv;  j++) {
    //Get the endpoints of the jth interval.
    wnfetd_c(&coverage, j, &begin, &end);
    //Convert the endpoints to TDB calendar
    begin -= startOffset;
    end += endOffset;
    timout_c(begin, calForm.c_str(), 35, begStr);
    timout_c(end, calForm.c_str(), 35, endStr);
    std::pair<std::string, std::string> p = {begStr, endStr};
    results.emplace_back(p);  
  }
  return results; 
}


std::vector<std::pair<std::string, std::string>> FormatFirstIntervals(SpiceCell &coverage, std::string type,
                                     double startOffset, double endOffset) {
  SpiceChar begStr[35], endStr[35];
  //Get the number of intervals in the object.
  int niv = card_c(&coverage) / 2;

  //Convert the coverage interval start and stop times to TDB
  double begin, end;
  
  std::vector<std::pair<std::string, std::string>> results;
  // std::cout << niv << std::endl; 
//   std::cout << niv << std::endl;
 
  if(niv != 0) {
    //Get the endpoints of the jth interval.
    wnfetd_c(&coverage, 0, &begin, &end);
    //Convert the endpoints to TDB calendar
    begin -= startOffset;
    end += endOffset;
    timout_c(begin, calForm.c_str(), 35, begStr);
    timout_c(end, calForm.c_str(), 35, endStr);
    
    std::pair<std::string, std::string> p = {begStr, endStr};
    results.emplace_back(p);  
    
    //Get the endpoints of the jth interval.
    wnfetd_c(&coverage, niv-1, &begin, &end);
    //Convert the endpoints to TDB calendar
    begin -= startOffset;
    end += endOffset;
    timout_c(begin, calForm.c_str(), 35, begStr);
    timout_c(end, calForm.c_str(), 35, endStr);
    
    p = {begStr, endStr};
  }    

  return results; 
}

int printcks() {
    fs::path kpath = "/data/spice/odyssey/kernels/ck/";
    std::vector<fs::path> kernels;

    double startOffset = 1;
    double endOffset = 1; 

    if (fs::exists(kpath) && fs::is_directory(kpath)) {
        for (auto const & entry : fs::directory_iterator(kpath)) {
            if (fs::is_regular_file(entry) && entry.path().extension() == ".bc")
                kernels.emplace_back(entry.path());
        }
    }
    std::cout << "n kernels: " << kernels.size() << std::endl;
    
    furnsh_c("/data/spice/odyssey/kernels/sclk/ORB1_SCLKSCET.00265.tsc");
    furnsh_c("/data/spice/lsk/naif0012.tls");

    fmt::print("strftime-like format: {}/{}/{}\n", 5, 5, 5);


    for(auto& p: kernels) {
        // std::cout << p << '\n';

        // std::cout << "furnshing" << std::endl; 
        // furnsh_c(p.string().c_str());
        // std::cout << "Done" << std::endl;
        
        SpiceChar fileType[32], source[2048];
        SpiceInt handle;
        SpiceBoolean found;

        kinfo_c(p.string().c_str(), 32, 2048, fileType, source, &handle, &found);
        std::string currFile = fileType;
        
        //create a spice cell capable of containing all the objects in the kernel.
        SPICEINT_CELL(currCell, 1000);
        //this resizing is done because otherwise a spice cell will append new data
        //to the last "currCell"
        ssize_c(0, &currCell);
        ssize_c(1000, &currCell);
        
        SPICEDOUBLE_CELL(cover, 200000); 
        // std::cout << "getting cov" << std::endl;

        ckcov_c(p.string().c_str(), -53000, SPICEFALSE, "SEGMENT", 0.0, "TDB", &cover);
        // std::cout << "done" << std::endl;

        std::vector<std::pair<std::string, std::string>> result;    
        result = FormatFirstIntervals(cover, currFile, startOffset, endOffset);
        
        // std::cout << "reading in ck obj" << std::endl;
        // ckobj_c(p.string().c_str(), &currCell);
        // std::cout << "done" << std::endl;

        // std::vector<std::pair<std::string, std::string>> result;        
        // std::cout << card_c(&currCell) << std::endl;
        // for(int bodyCount = 0 ; bodyCount < card_c(&currCell) ; bodyCount++) {
        //   //get the NAIF body code
        //   int body = SPICE_CELL_ELEM_I(&currCell, bodyCount);

        //   std::cout << "body: " << body << std::endl; 
        //   //only provide coverage for negative NAIF codes
        //   //(Positive codes indicate planetary bodies, negatives indicate
        //   // spacecraft and instruments)
        //   if (body < 0) {

        //     //find the correct coverage window
        //     if (currFile == "SPK") {
        //       std::cout << "getting cell" << std::endl;
        //       SPICEDOUBLE_CELL(cover, 200000);
        //       ssize_c(0, &cover);
        //       ssize_c(200000, &cover);
        //       spkcov_c(p.string().c_str(), body, &cover);                
        //       std::cout << "formatting" << std::endl;
        //       result = FormatFirstIntervals(cover, currFile, startOffset, endOffset);
        //     }
        //     else if (currFile == "CK") {
        //       std::cout << "getting cell" << std::endl;
        //       //  200,000 is the max coverage window size for a CK kernel
        //       SPICEDOUBLE_CELL(cover, 200000);
        //       ssize_c(0, &cover);
        //       ssize_c(200000, &cover);

        //       // A SPICE SEGMENT is composed of SPICE INTERVALS

        //       std::cout << "body: " << body << std::endl; 
        //       ckcov_c(p.string().c_str(), body, SPICEFALSE, "SEGMENT", 0.0, "TDB", &cover);
        //       
        //       std::cout << "formatting" << std::endl;
        //       result = FormatFirstIntervals(cover, currFile, startOffset, endOffset);
        //     }
        //   }
        // }

        for(auto& t: result) {
          std::cout << fmt::format(FMT_COMPILE("{}, {}\n"), t.first, t.second);    
        }

        // unload_c(p.string().c_str()); 
    }

}