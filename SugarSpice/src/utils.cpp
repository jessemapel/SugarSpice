/**
 *
 *
 *
 **/

#include <exception>

#include "utils.h"

using namespace std; 

string calForm = "YYYY MON DD HR:MN:SC.###### TDB ::TDB";

vector<fs::path> glob(fs::path const & root, string const & ext) {
    vector<fs::path> paths;

    if (fs::exists(root) && fs::is_directory(root)) {
        for (auto const & entry : fs::recursive_directory_iterator(root)) {
            if (fs::is_regular_file(entry) && entry.path().extension() == ext)
                paths.emplace_back(entry.path().filename());
        }
    }

    return paths;
}             


vector<pair<string, string>> FormatIntervals(SpiceCell &coverage, string type,
                                     double startOffset, double endOffset) {
  SpiceChar begStr[35], endStr[35];
  //Get the number of intervals in the object.
  int niv = card_c(&coverage) / 2;
  //Convert the coverage interval start and stop times to TDB
  double begin, end;
  
  vector<pair<string, string>> results;
  // cout << niv << endl; 
  for(int j = 0;  j < niv;  j++) {
    //Get the endpoints of the jth interval.
    wnfetd_c(&coverage, j, &begin, &end);
    //Convert the endpoints to TDB calendar
    begin -= startOffset;
    end += endOffset;
    timout_c(begin, calForm.c_str(), 35, begStr);
    timout_c(end, calForm.c_str(), 35, endStr);
    pair<string, string> p = {begStr, endStr};
    results.emplace_back(p);  
  }
  return results; 
}


vector<pair<string, string>> FormatFirstLastIntervals(SpiceCell &coverage, string type,
                                     double startOffset, double endOffset) {
  SpiceChar begStr[35], endStr[35];
  //Get the number of intervals in the object.
  int niv = card_c(&coverage) / 2;

  //Convert the coverage interval start and stop times to TDB
  double begin, end;
  
  vector<pair<string, string>> results;
 
  if(niv != 0) {
    //Get the endpoints of the jth interval.
    wnfetd_c(&coverage, 0, &begin, &end);
    //Convert the endpoints to TDB calendar
    begin -= startOffset;
    end += endOffset;
    timout_c(begin, calForm.c_str(), 35, begStr);
    timout_c(end, calForm.c_str(), 35, endStr);
    
    pair<string, string> p = {begStr, endStr};
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


vector<pair<string, string>> getCkIntervals(string kpath, string sclk, string lsk) {
    vector<fs::path> kernels;

    double startOffset = 1;
    double endOffset = 1; 

    if (fs::exists(kpath) && fs::is_directory(kpath)) {
        for (auto const & entry : fs::directory_iterator(kpath)) {
            if (fs::is_regular_file(entry) && entry.path().extension() == ".bc")
                kernels.emplace_back(entry.path());
        }
    }
    
    furnsh_c(sclk.c_str());
    furnsh_c(lsk.c_str());
    vector<pair<string, string>> result; 
    for(auto& p: kernels) {
        // furnsh_c(p.string().c_str());
        
        SpiceChar fileType[32], source[2048];
        SpiceInt handle;
        SpiceBoolean found;

        kinfo_c(p.string().c_str(), 32, 2048, fileType, source, &handle, &found);
        string currFile = fileType;
        
        //create a spice cell capable of containing all the objects in the kernel.
        SPICEINT_CELL(currCell, 1000);
        //this resizing is done because otherwise a spice cell will append new data
        //to the last "currCell"
        ssize_c(0, &currCell);
        ssize_c(1000, &currCell);
        
        SPICEDOUBLE_CELL(cover, 200000); 

        ckcov_c(p.string().c_str(), -53000, SPICEFALSE, "SEGMENT", 0.0, "TDB", &cover);

        ckobj_c(p.string().c_str(), &currCell);

        vector<pair<string, string>> result;        
        cout << card_c(&currCell) << endl;
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
              result = FormatFirstLastIntervals(cover, currFile, startOffset, endOffset);
            }
            else if (currFile == "CK") {
              //  200,000 is the max coverage window size for a CK kernel
              SPICEDOUBLE_CELL(cover, 200000);
              ssize_c(0, &cover);
              ssize_c(200000, &cover);

              // A SPICE SEGMENT is composed of SPICE INTERVALS
              ckcov_c(p.string().c_str(), body, SPICEFALSE, "SEGMENT", 0.0, "TDB", &cover);
              
              result = FormatFirstLastIntervals(cover, currFile, startOffset, endOffset);
            }
          }
        }

        for(auto& t: result) {
          cout << fmt::format(FMT_COMPILE("{}, {}\n"), t.first, t.second);    
        }

    }
    
    unload_c(sclk.c_str());
    unload_c(lsk.c_str()); 
    return result; 
}


fs::path getKernelDir(fs::path root, string mission, string instrument, KernelType type) {
  return "";
}


int getFrameCode(string body) {
  SpiceInt code; 
  SpiceBoolean found; 

  bodn2c_c(body.c_str(), &code, &found);

  if (!found) {
    throw "Body Code not Found";
  }

  return code;
}


string getKernelType(fs::path kernelPath) {
  SpiceChar type[6]; 
  SpiceChar source[6]; 
  SpiceInt handle; 
  SpiceBoolean found; 

  furnsh_c(kernelPath.c_str());

  kinfo_c(kernelPath.c_str(), 6, 6, type, source, &handle, &found);

  if (!found) {
    return "Kernel Type not found";
  }

  unload_c(kernelPath.c_str()); 
  return string(type);

}


