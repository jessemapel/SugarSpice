#include <iostream>

#include <ghc/fs_std.hpp>
#include <SpiceUsr.h>

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


int main() {
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

    for(auto& p: kernels) {
        // std::cout << p << '\n';
        
        furnsh_c("/data/spice/odyssey/kernels/sclk/ORB1_SCLKSCET.00265.tsc");
        furnsh_c("/data/spice/lsk/naif0012.tls");
        furnsh_c(p.string().c_str());
        
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

        ckobj_c(p.string().c_str(), &currCell);

        std::vector<std::pair<std::string, std::string>> result;        

        for(int bodyCount = 0 ; bodyCount < card_c(&currCell) ; bodyCount++) {
          //get the NAIF body code
          int body = SPICE_CELL_ELEM_I(&currCell, bodyCount);

          //only provide coverage for negative NAIF codes
          //(Positive codes indicate planetary bodies, negatives indicate
          // spacecraft and instruments)
          if (body < 0) {

            //find the correct coverage window
            if (currFile == "SPK") {
              SPICEDOUBLE_CELL(cover, 200000);
              ssize_c(0, &cover);
              ssize_c(200000, &cover);
              spkcov_c(p.string().c_str(), body, &cover);

              result = FormatIntervals(cover, currFile, startOffset, endOffset);
            }
            else if (currFile == "CK") {
              //  200,000 is the max coverage window size for a CK kernel
              SPICEDOUBLE_CELL(cover, 200000);
              ssize_c(0, &cover);
              ssize_c(200000, &cover);

              // A SPICE SEGMENT is composed of SPICE INTERVALS
              ckcov_c(p.string().c_str(), body, SPICEFALSE, "SEGMENT", 0.0, "TDB", &cover);

              result = FormatIntervals(cover, currFile, startOffset, endOffset);
            }
          }
        }

        // for(auto& t: result) {
            // std::cout << t.first << "\n" << t.second << std::endl;
        // }

        unload_c(p.string().c_str()); 
    }

}