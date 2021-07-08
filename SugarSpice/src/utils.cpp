/**
 *
 *
 *
 **/

#include <exception>
#include <fstream>

#include <SpiceUsr.h>
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
    // accumulate results to json-parseable string resBuffer
    
    // Define gXpool params
    ConstSpiceChar *fkey;
    SpiceInt nvals;
    SpiceChar cvals [ROOM][LENOUT];
    SpiceDouble dvals[ROOM];
    SpiceInt ivals[ROOM];
    SpiceBoolean gcfound, gdfound, gifound;

    json allResults;
    string resultVal;
    string resBuffer = "{";

    // iterate over kvals;
    for(int i = 0; i < nkeys; i++) {

      fkey = &kvals[i][0];
      gcpool_c(fkey, START, ROOM, LENOUT, &nvals, cvals, &gcfound);
      if(!gcfound) {
        gdpool_c(fkey, START, ROOM, &nvals, dvals, &gdfound);
        if(!gdfound) {
          gipool_c(fkey, START, ROOM, &nvals, ivals, &gifound);
          
          // format gipool output
          // all output values are formatted as a json list
          // including single values
          resultVal = "";
          for(int j=0; j<nvals-1; j++) {
            resultVal += fmt::format("{}, ", ivals[j]);
          }
          resultVal += fmt::format("{}", ivals[nvals-1]);
          // end gipool
        } else {
          
          // format gdpool output
          resultVal = "";
          for(int j=0; j<nvals-1; j++) {
            resultVal += fmt::format("{}, ", dvals[j]);
          }
          resultVal += fmt::format("{}", dvals[nvals-1]);
          // end gdpool 
        }
      } else {
        
        // format gcpool output
        resultVal = "";
        string str_cval;
        for(int j=0; j<nvals-1; j++) {
          str_cval.assign(&cvals[j][0]);
          resultVal += fmt::format("\"{}\", ", str_cval);
        }
        str_cval.assign(&cvals[nvals-1][0]);
        resultVal += fmt::format("\"{}\"", str_cval);
        // end gcpool
      }

      // append to resBuffer:
      //     key:list-of-values
      string resultKey(fkey);
      if(i < nkeys - 1) {
        resBuffer += fmt::format("\"{}\":[{}], ", resultKey, resultVal);
      } else {
        resBuffer += fmt::format("\"{}\":[{}]", resultKey, resultVal); 
      }
    }
    resBuffer += "}";

    return json::parse(resBuffer);
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
      //Convert the coverage interval START and stop times to TDB
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
        std::vector<pair<double, double>> times;
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
    //  cout << fmt::format(FMT_COMPILE("START/stop: {}, {}\n"), t.first, t.second);
    //}

    return result;
  }


  fs::path getMissionConfigFile(string mission) {
    // If running tests or debugging locally
    fs::path debugDbPath = fs::absolute(_SOURCE_PREFIX) / "SugarSpice" / "db";
    fs::path installDbPath = fs::absolute(_INSTALL_PREFIX) / "etc" / "SugarSpice" / "db";

    fs::path dbPath = fs::exists(installDbPath) ? installDbPath : debugDbPath;

    if (!fs::is_directory(dbPath)) {
      throw "No Valid Path found";
    }

    std::vector<fs::path> paths = glob(dbPath, basic_regex("json"));

    for(auto p : paths) {
      if (p.filename() == fmt::format("{}.json", mission)) {
        return p;
      }
    }

    throw invalid_argument(fmt::format("Config file for \"{}\" not found", mission));
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
