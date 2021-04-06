/**
 *
 *
 *
 **/

#include <exception>

#include <SpiceUsr.h>
#include <nlohmann/json.hpp>

#include "utils.h"
#include "spice_types.h"

using json = nlohmann::json;
using namespace std; 

string calForm = "YYYY MON DD HR:MN:SC.###### TDB ::TDB";


// // partial specialization for fs::path
// namespace nlohmann {
//     template <typename T>
//     struct adl_serializer<fs::path> {
//         static void to_json(json& j, const fs::path& opt) {
//           j = opt.u8string();
//         }

//         static void from_json<fs::path>(const json& j, fs::path& opt) {
//                 opt = j.get<string>();
//         }
//     };
//}

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



vector<fs::path> glob(fs::path const & root, regex const & reg, bool recursive) {
    vector<fs::path> paths;

    if (fs::exists(root) && fs::is_directory(root)) {
        for (auto i = fs::recursive_directory_iterator(root); i != fs::recursive_directory_iterator(); ++i ) {
            if (fs::exists(*i) && regex_search(i->path().c_str(), reg)) {
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


vector<pair<string, string>> FormatIntervals(SpiceCell &coverage, string type,
                                     double startOffset, double endOffset) {
  SpiceChar begStr[35], endStr[35];
  //Get the number of intervals in the object.
  int niv = card_c(&coverage) / 2;
  //Convert the coverage interval start and stop times to TDB
  double begin, end;
  
  vector<pair<string, string>> results;
  
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


fs::path getDbFile(string mission) { 
    // If running tests or debugging locally 
    fs::path debugDbPath = fs::absolute(_SOURCE_PREFIX) / "SugarSpice" / "db";
    fs::path installDbPath = fs::absolute(_INSTALL_PREFIX) / "etc" / "SugarSpice" / "db";

    // use anaconda 

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

    throw invalid_argument(fmt::format("DB file for \"{}\" not found", mission));
}


json getMissionKernels(fs::path root, string mission) {
  auto parseDeps = [&](json db, vector<string> deps) -> json {
    return "";
  };

  auto getKernelsCkFromJson = [](json j, Kernel::Type t, Kernel::Quality q) -> vector<fs::path> {
    string sType = Kernel::translateType(t);
    string sQa = Kernel::translateQuality(q);

    // Start at the highest requested Quality, then try lower quality kernels if not available
    auto highestQa = Kernel::QUALITIES.begin() + static_cast<int>(q);
    for (auto it = highestQa; it == Kernel::QUALITIES.begin(); it--) {
      if (!j.contains(*it)) { 

      }
    }
    return {};
  };
  
  auto getPathsFromRegex = [&root](json r) -> vector<fs::path> {
      vector<string> regexes = jsonArrayToVector(r); 
      regex reg(fmt::format("({})", fmt::join(regexes, "|")));
      vector<fs::path> paths = glob(root, reg, true);

      return paths;
  };

  auto globCks = [&](json category) -> json {
    if(!category.contains("ck")) {
      return (json){};
    }

    category = category["ck"];
    json ret;
    
    for(auto qual: Kernel::QUALITIES) {
      if(!category.contains(qual)) {
        continue; 
      }
      ret[qual] = getPathsFromRegex(category[qual]);
    }

    // pass deps through
    if (category.at("deps").contains("objs")) {
      ret["deps"]["objs"] = category.at("deps").at("objs");
    }
    
    ret["deps"]["sclk"] = getPathsFromRegex(category.at("deps").at("sclk"));
    ret["deps"]["lsk"] = getPathsFromRegex(category.at("deps").at("lsk"));
    return ret;
  };

  auto globSpks = [&](json category) -> json {
    if(!category.contains("spk")) {
      return (json){};
    }
    
    category = category["ck"];
    json ret; 

    for(auto qual: Kernel::QUALITIES) {
      if(!category.contains(qual)) {
        continue; 
      }
      ret[qual] = getPathsFromRegex(category[qual]);
    }

    // pass deps through
    if (category.at("deps").contains("objs")) {
      ret["deps"]["objs"] = category.at("deps").at("objs");
    }

    ret["deps"]["sclk"] = getPathsFromRegex(category.at("deps").value("sclk", "$^"));
    ret["deps"]["lsk"] = getPathsFromRegex(category.at("deps").value("lsk", "$^"));
    std::cout << category << std::endl;    

    return ret; 
  };

  auto globFks = [&](json category) -> json {
    json ret; 
    std::cout << category << std::endl; 
    return getPathsFromRegex(category.value("fk", "$^"));
  };

  auto globIks = [&](json category) -> json {
    json ret; 
    return getPathsFromRegex(category.value("ik", "$^"));
  };

  auto globIaks = [&](json category) -> json {
    json ret; 
    return getPathsFromRegex(category.value("iak", "$^"));
  };

  auto globPcks = [&](json category) -> json {
    json ret; 
    return getPathsFromRegex(category.value("pck", "$^"));
  };

  fs::path dbPath = getDbFile(mission);
  
  ifstream i(dbPath);
  json db;
  i >> db;
  
  // first get any dependencies 
  // string deps = jsonArrayToVector(db[instrument][sType]); 

  json kernels; 

  // iterate the categories (e.g. missions)
  for (auto& cat: db.items()) {
      kernels[cat.key()]["ck"] = globCks(cat.value());
      kernels[cat.key()]["spk"] = globSpks(cat.value());
      kernels[cat.key()]["fk"] = globSpks(cat.value());
      kernels[cat.key()]["fk"] = globFks(cat.value());
      kernels[cat.key()]["ik"] = globIks(cat.value());
      kernels[cat.key()]["iak"] = globIaks(cat.value());
      kernels[cat.key()]["pck"] = globPcks(cat.value());
  }

  return kernels;
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


