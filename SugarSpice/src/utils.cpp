/**
 *
 *
 *
 **/

#include <exception>
#include <fstream>
#include <optional>

#include <SpiceUsr.h>
#include <SpiceZfc.h>
#include <SpiceZmc.h>

#include <nlohmann/json.hpp>

#include "utils.h"
#include "spice_types.h"

using json = nlohmann::json;
using namespace std;

string calForm = "YYYY MON DD HR:MN:SC.###### TDB ::TDB";

// FMT formatter for fs::path, this enables passing path objects to FMT calls.
template <> struct fmt::formatter<fs::path> {
  char presentation = 'f';

  constexpr auto parse(format_parse_context& ctx) {
    // Parse the presentation format and store it in the formatter:
    auto it = ctx.begin(), end = ctx.end();
    if (it != end && (*it == 'f' || *it == 'e')) presentation = *it++;

    // Check if reached the end of the range:
    if (it != end && *it != '}')
      throw format_error("invalid format");

    // Return an iterator past the end of the parsed range:
    return it;
  }
  
  template <typename FormatContext>
  auto format(const fs::path& p, FormatContext& ctx) {
  // auto format(const point &p, FormatContext &ctx) -> decltype(ctx.out()) // c++11
    // ctx.out() is an output iterator to write to.
    return format_to(
        ctx.out(),
        "{}",
        p.c_str());
  }
};


namespace SugarSpice {

  string toUpper(string s) {
    transform(s.begin(), s.end(), s.begin(), [](unsigned char c){ return toupper(c); });
    return s;
  }


  string toLower(string s) {
    transform(s.begin(), s.end(), s.begin(), [](unsigned char c){ return tolower(c); });
    return s;
  }


  string replaceAll(string str, const string& from, const string& to) {
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
    }
    return str;
  }


  targetState getTargetState(double et, string target, string observer, string frame, string abcorr) {    
    // convert params to spice types
    ConstSpiceChar *target_spice = target.c_str();  // better way to do this?
    ConstSpiceChar *observer_spice = observer.c_str();
    ConstSpiceChar *frame_spice = frame.c_str();
    ConstSpiceChar *abcorr_spice = abcorr.c_str();

    // define outputs
    SpiceDouble lt;
    SpiceDouble starg_spice[6];

    spkezr_c( target_spice, et, frame_spice, abcorr_spice, observer_spice, starg_spice, &lt );

    // convert to std::array for output
    array<double, 6> starg;
    for(int i = 0; i < 6; i++) {
      starg[i] = starg_spice[i];
    }

    return {lt, starg};
  }

  targetOrientation getTargetOrientation(double et, int toFrame, int refFrame) {
    // Much of this function is from ISIS SpiceRotation.cpp
    SpiceDouble stateCJ[6][6];
    SpiceDouble CJ_spice[3][3];
    SpiceDouble av_spice[3];
    SpiceDouble quat_spice[4];

    array<double,4> quat;
    array<double,3> av;

    bool has_av = true;

    // First try getting the entire state matrix (6x6), which includes CJ and the angular velocity
    frmchg_((int *) &refFrame, (int *) &toFrame, &et, (doublereal *) stateCJ);

    if (!failed_c()) {
      // Transpose and isolate CJ and av
      xpose6_c(stateCJ, stateCJ);
      xf2rav_c(stateCJ, CJ_spice, av_spice);

      // Convert to std::array for output
      for(int i = 0; i < 3; i++) {
        av[i] = av_spice[i];
      }

    }
    else {  // TODO This case is untested
      // Recompute CJ_spice ignoring av
      
      reset_c(); // reset frmchg_ failure
       
      refchg_((int *) &refFrame, (int *) &toFrame, &et, (doublereal *) CJ_spice);
      xpose_c(CJ_spice, CJ_spice);

      has_av = false;
    }

    // Translate matrix to std:array quaternion
    m2q_c(CJ_spice, quat_spice);
    for(int i = 0; i < 4; i++) {
      quat[i] = quat_spice[i];
    }

    if(has_av) return {quat, av};
    return {quat, nullopt};
  }


  // Given a string keyname template, search the kernel pool for matching keywords and their values
  // returns json with up to ROOM=50 matching keynames:values
  // if no keys are found, returns null
  json findKeywords(string keytpl) {
    // Define gnpool i/o
    const SpiceInt START = 0;
    const SpiceInt ROOM = 50;
    const SpiceInt LENOUT = 100;
    ConstSpiceChar *cstr = keytpl.c_str();
    SpiceInt nkeys;
    SpiceChar kvals [ROOM][LENOUT];
    SpiceBoolean gnfound;

    // Call gnpool to search for input key template
    gnpool_c(cstr, START, ROOM, LENOUT, &nkeys, kvals, &gnfound);

    if(!gnfound) {
      return nullptr;
    }

    // Call gXpool for each key found in gnpool
    // accumulate results to json allResults
    
    // Define gXpool params
    ConstSpiceChar *fkey;
    SpiceInt nvals;
    SpiceChar cvals [ROOM][LENOUT];
    SpiceDouble dvals[ROOM];
    SpiceInt ivals[ROOM];
    SpiceBoolean gcfound = false, gdfound = false, gifound = false;
    
    json allResults;

    // iterate over kvals;
    for(int i = 0; i < nkeys; i++) {  
      json jresultVal;

      fkey = &kvals[i][0];

      gdpool_c(fkey, START, ROOM, &nvals, dvals, &gdfound); 
      
      if (gdfound) {
        // format output
        if (nvals == 1) {
          jresultVal = dvals[0]; 
        }
        else {
          for(int j=0; j<nvals; j++) {
            jresultVal.push_back(dvals[j]);
          }
        }
      }

      if (!gdfound) {
        gipool_c(fkey, START, ROOM, &nvals, ivals, &gifound);
      }

      if (gifound) {
        // format output
        if (nvals == 1) {
          jresultVal = ivals[0]; 
        }
        else {
          for(int j=0; j<nvals; j++) {
            jresultVal.push_back(ivals[j]);
          }
        }
      }

      if (!gifound || !gdfound) { 
        gcpool_c(fkey, START, ROOM, LENOUT, &nvals, cvals, &gcfound);
      }

      if (gcfound) {
        // format gcpool output
        string str_cval;
        if (nvals == 1) {
          str_cval.assign(&cvals[0][0]);
          string lower = toLower(str_cval);

          // if null or boolean, do a conversion 
          if (lower == "true") {
            jresultVal = true;
          }
          else if (lower == "false") {
            jresultVal = false;
          } 
          else if (lower == "null") {
            jresultVal = nullptr;
          }
          else {
            jresultVal = str_cval; 
          }
        }
        else {
          for(int j=0; j<nvals; j++) {
            str_cval.assign(&cvals[j][0]);
            string lower = toLower(str_cval);
            
            // if null or boolean, do a conversion 
            if (lower == "true") {
              jresultVal.push_back(true);
            }
            else if (lower == "false") {
              jresultVal.push_back(false);
            } 
            else if (lower == "null") {
              jresultVal.push_back(nullptr);
            }
            else {
              jresultVal.push_back(str_cval);
            }
          }
        }
      }
      
      // append to allResults:
      //     key:list-of-values
      string resultKey(fkey);
      allResults[resultKey] = jresultVal;
    }

    return allResults;
  }

  vector<json::json_pointer> findKeyInJson(json in, string key, bool recursive) {
    function<vector<json::json_pointer>(json::json_pointer, string, vector<json::json_pointer>, bool)> recur = [&recur, &in](json::json_pointer elem, string key, vector<json::json_pointer> vec, bool recursive) -> vector<json::json_pointer> {
      json e = in[elem];
      for (auto &it : e.items()) {
        json::json_pointer pointer = elem/it.key();
        if (recursive && it.value().is_structured()) {
          vec = recur(pointer, key, vec, recursive);
        }
        if(it.key() == key) {
          vec.push_back(pointer);
        }
      }
      return vec;
    };

    vector<json::json_pointer> res;
    json::json_pointer p = ""_json_pointer;
    res = recur(p, key, res, recursive);
    return res;
  }

  vector<string> jsonArrayToVector(json arr) {
    vector<string> res;
    
    if (arr.is_array()) {
      for(auto it : arr) {
        res.emplace_back(it);
      }
    }
    else if (arr.is_string()) {
      res.emplace_back(arr);
    }
    else {
      throw invalid_argument("Input json is not a valid Json array");
    }

    return res;
  }


  vector<fs::path> ls(fs::path const & root, bool recursive) {
    vector<fs::path> paths;

    if (fs::exists(root) && fs::is_directory(root)) {
      for (auto i = fs::recursive_directory_iterator(root); i != fs::recursive_directory_iterator(); ++i ) {
        if (fs::exists(*i)) {
          paths.emplace_back(i->path());
        }

        if(!recursive) {
          // simply disable recursion if recurse flag is off
          i.disable_recursion_pending();
        }
      }
    }

    return paths;
  }


  vector<fs::path> glob(fs::path const & root, regex const & reg, bool recursive) {
    vector<fs::path> paths;
    vector<fs::path> files_to_search = ls(root, recursive);

    for (auto &f : files_to_search) {
      if (regex_search(f.c_str(), reg)) {
        paths.emplace_back(f);
      }
    }

    return paths;
  }


  vector<pair<double, double>> getTimeIntervals(fs::path kpath) {
    auto formatIntervals = [&](SpiceCell &coverage) -> vector<pair<double, double>> {
      //Get the number of intervals in the object.
      int niv = card_c(&coverage) / 2;
      //Convert the coverage interval start and stop times to TDB
      double begin, end;

      vector<pair<double, double>> res;

      for(int j = 0;  j < niv;  j++) {
        //Get the endpoints of the jth interval.
        wnfetd_c(&coverage, j, &begin, &end);

        pair<double, double> p = {begin, end};
        res.emplace_back(p);
      }

      return res;
    };


    SpiceChar fileType[32], source[2048];
    SpiceInt handle;
    SpiceBoolean found;

    Kernel k(kpath);

    kinfo_c(kpath.string().c_str(), 32, 2048, fileType, source, &handle, &found);
    string currFile = fileType;

    //create a spice cell capable of containing all the objects in the kernel.
    SPICEINT_CELL(currCell, 1000);

    //this resizing is done because otherwise a spice cell will append new data
    //to the last "currCell"
    ssize_c(0, &currCell);
    ssize_c(1000, &currCell);

    SPICEDOUBLE_CELL(cover, 200000);

    if (currFile == "SPK") {
      spkobj_c(kpath.string().c_str(), &currCell);
    }
    else if (currFile == "CK") {
      ckobj_c(kpath.string().c_str(), &currCell);
    }
    else if (currFile == "TEXT") {
      throw invalid_argument("Input Kernel is a text kernel which has no intervals");
    }

    vector<pair<double, double>> result;

    for(int bodyCount = 0 ; bodyCount < card_c(&currCell) ; bodyCount++) {
      //get the NAIF body code
      int body = SPICE_CELL_ELEM_I(&currCell, bodyCount);

      //only provide coverage for negative NAIF codes
      //(Positive codes indicate planetary bodies, negatives indicate
      // spacecraft and instruments)
      if (body < 0) {
        vector<pair<double, double>> times;
        //find the correct coverage window
        if(currFile == "SPK") {
          SPICEDOUBLE_CELL(cover, 200000);
          ssize_c(0, &cover);
          ssize_c(200000, &cover);
          spkcov_c(kpath.string().c_str(), body, &cover);
          times = formatIntervals(cover);
        }
        else if(currFile == "CK") {
          //  200,000 is the max coverage window size for a CK kernel
          SPICEDOUBLE_CELL(cover, 200000);
          ssize_c(0, &cover);
          ssize_c(200000, &cover);

          // A SPICE SEGMENT is composed of SPICE INTERVALS
          ckcov_c(kpath.string().c_str(), body, SPICEFALSE, "SEGMENT", 0.0, "TDB", &cover);

          times = formatIntervals(cover);
        }

        result.reserve(result.size() + distance(times.begin(), times.end()));
        result.insert(result.end(), times.begin(), times.end());
      }
    }

    //for(auto& t: result) {
    //  cout << fmt::format(FMT_COMPILE("start/stop: {}, {}\n"), t.first, t.second);
    //}

    return result;
  }


  fs::path getDataDirectory() {
      char* ptr = getenv("ISISDATA");
      fs::path isisDataDir = ptr == NULL ? "" : ptr;
  
      ptr = getenv("ALESPICEROOT");
      fs::path aleDataDir = ptr == NULL ? "" : ptr;
  
      ptr = getenv("SPICEROOT");
      fs::path spiceDataDir = ptr == NULL ? "" : ptr;
  
      if (fs::is_directory(spiceDataDir)) {
         return spiceDataDir;
      }
  
      if (fs::is_directory(aleDataDir)) {
        return aleDataDir; 
      }
  
      if (fs::is_directory(isisDataDir)) {
        return isisDataDir; 
      }  
      
      throw runtime_error(fmt::format("Please set env var SPICEROOT, ISISDATA or ALESPICEROOT in order to proceed."));
  }


  fs::path getMissionConfigFile(string mission) {
    // If running tests or debugging locally
    char* condaPrefix = std::getenv("CONDA_PREFIX");

    fs::path debugDbPath = fs::absolute(_SOURCE_PREFIX) / "SugarSpice" / "db";
    fs::path installDbPath = fs::absolute(condaPrefix) / "etc" / "SugarSpice" / "db";

    // Use installDbPath unless $SSPICE_DEBUG is set
    fs::path dbPath = std::getenv("SSPICE_DEBUG") ? debugDbPath : installDbPath;

    if (!fs::is_directory(dbPath)) {
      throw runtime_error("Config Directory Not Found.");
    }

    vector<fs::path> paths = glob(dbPath, basic_regex("json"));

    for(auto p : paths) {
      if (p.filename() == fmt::format("{}.json", mission)) {
        return p;
      }
    }

    throw invalid_argument(fmt::format("Config file for \"{}\" not found", mission));
  }


  json getMissionConfig(string mission) { 
    fs::path dbPath = getMissionConfigFile(mission);
  
    ifstream i(dbPath);
    json conf;
    i >> conf;
    return conf; 
  }

  string getKernelType(fs::path kernelPath) {
    SpiceChar type[6];
    SpiceChar source[6];
    SpiceInt handle;
    SpiceBoolean found;

    furnsh_c(kernelPath.c_str());

    kinfo_c(kernelPath.c_str(), 6, 6, type, source, &handle, &found);

    if (!found) {
      throw domain_error("Kernel Type not found");
    }

    unload_c(kernelPath.c_str());
    return string(type);
  }

}

