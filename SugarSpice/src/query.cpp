/**
  * @file
  *
  *
  *
  *
 **/

#include <fstream>

#include <SpiceUsr.h>

#include "query.h"
#include "spice_types.h"
#include "utils.h"

using json = nlohmann::json;
using namespace std;

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


json searchMissionKernels(fs::path root, json conf) {

  /**
    * Lambda for parsing a CK json object, returns a json object
    * with a similar structure, but regexes replaces with a path list
   **/
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
      ret[qual]["kernels"] = getPathsFromRegex(root, category[qual].at("kernels"));
      if (category[qual].at("deps").contains("lsk")) {
        ret[qual]["deps"]["lsk"] = getPathsFromRegex(root, category[qual].at("deps").at("lsk"));
      }
      if (category[qual].at("deps").contains("sclk")) {
        ret[qual]["deps"]["sclk"] = getPathsFromRegex(root, category[qual].at("deps").at("sclk"));
      }
      if (category[qual].at("deps").contains("pck")) {
        ret[qual]["deps"]["pck"] = getPathsFromRegex(root, category[qual].at("deps").at("pck"));
      }
      if (category[qual].at("deps").contains("objs")) {
        ret[qual]["deps"]["objs"] = category[qual].at("deps").at("objs");
      }
    }

    // pass deps through
    if (category.at("deps").contains("objs")) {
      ret["deps"]["objs"] = category.at("deps").at("objs");
    }

    ret["deps"]["sclk"] = getPathsFromRegex(root, category.at("deps").at("sclk"));
    ret["deps"]["lsk"] = getPathsFromRegex(root, category.at("deps").at("lsk"));
    return ret;
  };

  /**
    * Lambda for parsing a SPK json object, returns a json object
    * with a similar structure, but regexes replaces with a path list
   **/
  auto globSpks = [&](json category) -> json {
    if(!category.contains("spk")) {
      return (json){};
    }

    category = category["spk"];
    json ret;

    for(auto qual: Kernel::QUALITIES) {
      if(!category.contains(qual)) {
        continue;
      }
      ret[qual]["kernels"] = getPathsFromRegex(root, category[qual].at("kernels"));
      if (category[qual].at("deps").contains("lsk")) {
        ret[qual]["deps"]["lsk"] = getPathsFromRegex(root, category[qual].at("deps").at("lsk"));
      }
      if (category[qual].at("deps").contains("sclk")) {
        ret[qual]["deps"]["sclk"] = getPathsFromRegex(root, category[qual].at("deps").at("sclk"));
      }
      if (category[qual].at("deps").contains("objs")) {
        ret[qual]["deps"]["objs"] = category[qual].at("deps").at("objs");
      }
    }

    // pass deps through
    if (category.at("deps").contains("objs")) {
      ret["deps"]["objs"] = category.at("deps").at("objs");
    }

    ret["deps"]["sclk"] = getPathsFromRegex(root, category.at("deps").value("sclk", "$^"));
    ret["deps"]["lsk"] = getPathsFromRegex(root, category.at("deps").value("lsk", "$^"));

    return ret;
  };


  /**
    * Lambda for parsing a SPK json object, returns a json object
    * with a similar structure, but regexes replaces with a path list
   **/
  auto globTspks = [&](json category) -> json {
    if(!category.contains("tspk")) {
      return (json){};
    }

    category = category["tspk"];
    json ret;

    for(auto qual: Kernel::QUALITIES) {
      if(!category.contains(qual)) {
        continue;
      }
      ret[qual] = getPathsFromRegex(root, category[qual]);
    }

    if (category.contains("deps")) {
        // pass deps through
        if (category.at("deps").contains("objs")) {
            ret["deps"]["objs"] = category.at("deps").at("objs");
        }

        ret["deps"]["sclk"] = getPathsFromRegex(root, category.at("deps").value("sclk", "$^"));
        ret["deps"]["lsk"] = getPathsFromRegex(root, category.at("deps").value("lsk", "$^"));
    }

    return ret;
  };


  /**
    * Lambda for parsing a FK json object, returns a json object
    * with a similar structure, but regexes replaces with a path list
   **/
  auto globFks = [&](json category) -> json {
    json ret;
    return getPathsFromRegex(root, category.value("fk", "$^"));
  };

  /**
    * Lambda for parsing a IK json object, returns a json object
    * with a similar structure, but regexes replaces with a path list
   **/
  auto globIks = [&](json category) -> json {
    json ret;
    return getPathsFromRegex(root, category.value("ik", "$^"));
  };

  /**
    * Lambda for parsing a IAK json object, returns a json object
    * with a similar structure, but regexes replaces with a path list
   **/
  auto globIaks = [&](json category) -> json {
    json ret;
    return getPathsFromRegex(root, category.value("iak", "$^"));
  };

  /**
    * Lambda for parsing a PCK json object, returns a json object
    * with a similar structure, but regexes replaces with a path list
   **/
  auto globPcks = [&](json category) -> json {
    if(!category.contains("pck")) {
      return (json){};
    }

    category = category["pck"];
    json ret;

    for(auto qual: Kernel::QUALITIES) {
      if(!category.contains(qual)) {
        continue;
      }
      ret[qual]["kernels"] = getPathsFromRegex(root, category[qual]["kernels"]);
    }

    if (category.contains("deps")) {
        // pass deps through
        if (category.at("deps").contains("objs")) {
            ret["deps"]["objs"] = category.at("deps").at("objs");
        }

        ret["deps"]["sclk"] = getPathsFromRegex(root, category.at("deps").value("sclk", "$^"));
        ret["deps"]["lsk"] = getPathsFromRegex(root, category.at("deps").value("lsk", "$^"));
    }

    return ret;
  };


  // first get any dependencies
  // string deps = jsonArrayToVector(db[instrument][sType]);

  json kernels;

  // iterate the categories (e.g. missions)
  for (auto& cat: conf.items()) {
      kernels[cat.key()]["ck"] = globCks(cat.value());
      kernels[cat.key()]["spk"] = globSpks(cat.value());
      kernels[cat.key()]["tspk"] = globTspks(cat.value());
      kernels[cat.key()]["fk"] = globFks(cat.value());
      kernels[cat.key()]["ik"] = globIks(cat.value());
      kernels[cat.key()]["iak"] = globIaks(cat.value());
      kernels[cat.key()]["pck"] = globPcks(cat.value());
  }

  return kernels;
}



json searchMissionKernels(json kernels, std::vector<double> times, bool isContiguous)  {
  auto loadTimeKernels = [&](json j) -> vector<shared_ptr<Kernel>> {
    // get sclk
    vector<json::json_pointer> p = findKeyInJson(j, "sclk", true);

    vector<string> sclks = jsonArrayToVector(j[p.at(0)]);
    sort(sclks.begin(), sclks.end(), greater<string>());

    // get lsk
    p = findKeyInJson(j, "lsk", true);

    vector<string> lsks = jsonArrayToVector(j[p.at(0)]);
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

      json ckQual = cks[qual];
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

      reducedKernels[p/qual] = newKernels;
      reducedKernels[p]["deps"] = kernels[p]["deps"];
    }
  }

  return reducedKernels;
}
