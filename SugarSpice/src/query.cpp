/**
  * @file
  *
  *
  *
  *
 **/

#include <fstream>
#include <algorithm>

#include <SpiceUsr.h>

#include "query.h"
#include "spice_types.h"
#include "utils.h"

using json = nlohmann::json;
using namespace std;

namespace SugarSpice {
  
  fs::path getLatestKernel(vector<string> kernels) {
    string extension = static_cast<fs::path>(kernels.at(0)).extension();
    // ensure everything is different versions of the same file
    for(const fs::path &k : kernels) {
      if (k.extension() != extension) {
        throw invalid_argument("The input paths do are not different versions of the same file");
      }
    }
  
    return *(max_element(kernels.begin(), kernels.end()));
  }
  
  
  json getLatestKernels(json kernels) {
    // the kernels group is now the conf with 
    for(auto &kernelType: {"ck", "spk", "tspk", "fk", "ik", "iak", "pck", "lsk"}) {
        vector<json::json_pointer> catPointers = findKeyInJson(kernels, kernelType, true);
        for(auto &p : catPointers) {
          for(auto qual: Kernel::QUALITIES) { 
            if(!kernels[p].contains(qual)){
              continue; 
            }
            std::vector<string> l = jsonArrayToVector(kernels[p][qual]["kernels"]);
            fs::path latest;

            if (!l.empty()) {
              latest = getLatestKernel(l);
              kernels[p][qual]["kernels"] = latest; 
            }
          }
          
          if(kernels[p].contains("kernels")) { 
            fs::path latest = getLatestKernel(jsonArrayToVector(kernels[p]["kernels"]));
            kernels[p]["kernels"] = latest;  
          }
        }
    }

    vector<json::json_pointer> pointers = findKeyInJson(kernels, "sclk", true);
    for(auto &p : pointers) {
      fs::path latest = getLatestKernel(jsonArrayToVector(kernels[p]));
      kernels[p] = latest; 
    }
 
    return kernels;
  }

  /**
    * @brief glob, but with json
    *
    * Lambda for globbing files from a regular expression stored
    * in json. As they can be a single expression or a
    * list, we need to massage the json a little.
    *
    * @param root root path to search
    * @param r json list of regexes
    * @returns vector of paths
   **/
  vector<fs::path> getPathsFromRegex (fs::path root, json r) {
      vector<string> regexes = jsonArrayToVector(r);
      regex reg(fmt::format("({})", fmt::join(regexes, "|")));
      vector<fs::path> paths = glob(root, reg, true);
  
      return paths;
  };
  
  
  json globKernels(fs::path root, json conf, string kernelType) {
    vector<json::json_pointer> pointers = findKeyInJson(conf, kernelType, true);
  
    json ret; 
  
    // iterate pointers
    for(auto pointer : pointers) {      
      json category = conf[pointer];
  
      if (category.contains("kernels")) {
        ret[pointer]["kernels"] = getPathsFromRegex(root, category.at("kernels"));
      }
  
      if (category.contains("deps")) {
        if (category.at("deps").contains("sclk")) {
          ret[pointer]["deps"]["sclk"] = getPathsFromRegex(root, category.at("deps").at("sclk"));
        }
        if (category.at("deps").contains("pck")) {
          ret[pointer]["deps"]["pck"] = getPathsFromRegex(root, category.at("deps").at("pck"));
        }
        if (category.at("deps").contains("objs")) {
          ret[pointer]["deps"]["objs"] = category.at("deps").at("objs");
        }
      }
  
      // iterate over potential qualities 
      for(auto qual: Kernel::QUALITIES) {
        if(!category.contains(qual)) {
          continue;
        }
        
        ret[pointer][qual]["kernels"] = getPathsFromRegex(root, category[qual].at("kernels"));
        
        if (category[qual].contains("deps")) {
          if (category[qual].at("deps").contains("sclk")) {
            ret[pointer][qual]["deps"]["sclk"] = getPathsFromRegex(root, category[qual].at("deps").at("sclk"));
          }
          if (category[qual].at("deps").contains("pck")) {
            ret[pointer][qual]["deps"]["pck"] = getPathsFromRegex(root, category[qual].at("deps").at("pck"));
          }
          if (category[qual].at("deps").contains("objs")) {
            ret[pointer][qual]["deps"]["objs"] = category[qual].at("deps").at("objs");
          }
        }
      }
    }
  
    return  ret.empty() ? "{}"_json : ret;
  }
  
  
  json searchMissionKernels(fs::path root, json conf) {
    json kernels;
  
    // the kernels group is now the conf with 
    for(auto &kernelType: {"ck", "spk", "tspk", "fk", "ik", "iak", "pck", "lsk"}) {
        kernels.merge_patch(globKernels(root, conf, kernelType));
    }
  
    return kernels;
  }
  
  
  json searchMissionKernels(json kernels, std::vector<double> times, bool isContiguous)  {
    auto loadTimeKernels = [&](json j) -> vector<shared_ptr<Kernel>> {
      vector<json::json_pointer> p = findKeyInJson(j, "sclk", true);
      vector<string> sclks;

      if (!p.empty()) { 
        sclks = jsonArrayToVector(j[p.at(0)]);
        sort(sclks.begin(), sclks.end(), greater<string>());
      }
  
      json baseConf = getMissionConfig("base");
      fs::path dataDir = getDataDirectory();
      
      baseConf = searchMissionKernels(dataDir, baseConf);
      p = findKeyInJson(baseConf, "lsk", true);
      
      vector<string> lsks = jsonArrayToVector(baseConf.at(p.at(0))["kernels"]);
      sort(lsks.begin(), lsks.end(), greater<string>());
      
      vector<shared_ptr<Kernel>> timeKernels(2);
  
      if(lsks.size()) {
        timeKernels.emplace_back(new Kernel(lsks.at(0)));
      }
      if (sclks.size()) {
        timeKernels.emplace_back(new Kernel(sclks.at(0)));
      }
      return timeKernels;
    };
  
    json reducedKernels;
  
    vector<json::json_pointer> ckpointers = findKeyInJson(kernels, "ck", true);
    vector<json::json_pointer> spkpointers = findKeyInJson(kernels, "spk", true);
    vector<json::json_pointer> pointers(ckpointers.size() + spkpointers.size());
    merge(ckpointers.begin(), ckpointers.end(), spkpointers.begin(), spkpointers.end(), pointers.begin());
  
    json newKernels = json::array();
  
    // refine cks for every instrument/category
    for (auto &p : pointers) {
      json cks = kernels[p];
      if(cks.is_null() ) {
        continue;
      }
  
      // load time kernels
      vector<shared_ptr<Kernel>> timeKernels = loadTimeKernels(cks);
      
      for(auto qual: Kernel::QUALITIES) {
        if(!cks.contains(qual)) {
          continue;
        }
  
        json ckQual = cks[qual]["kernels"];
        newKernels = json::array();
        
        for(auto &kernel : ckQual) {
          vector<pair<double, double>> intervals = getTimeIntervals(kernel);

          for(auto &interval : intervals) {
            auto isInRange = [&interval](double d) -> bool {return d >= interval.first && d <= interval.second;};
  
            if (isContiguous && all_of(times.cbegin(), times.cend(), isInRange)) {
              newKernels.push_back(kernel);
            }
            else if (any_of(times.cbegin(), times.cend(), isInRange)) {
              newKernels.push_back(kernel);
            }
          } // end of searching intervals
        } // end  of searching kernels
        
        reducedKernels[p/qual/"kernels"] = newKernels;
        reducedKernels[p]["deps"] = kernels[p]["deps"];
      }
    }
    kernels.merge_patch(reducedKernels);
    return kernels;
  }
}