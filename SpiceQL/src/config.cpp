#include "config.h"

#include <fstream>

#include <ghc/fs_std.hpp>

using namespace std;
using json = nlohmann::json;

namespace SpiceQL {

  Config::Config() {
    // If running tests or debugging locally
    char* condaPrefix = std::getenv("CONDA_PREFIX");

    fs::path debugDbPath = fs::absolute(_SOURCE_PREFIX) / "SpiceQL" / "db";
    fs::path installDbPath = fs::absolute(condaPrefix) / "etc" / "SpiceQL" / "db";

    // Use installDbPath unless $SSPICE_DEBUG is set
    fs::path dbPath = std::getenv("SSPICE_DEBUG") ? debugDbPath : installDbPath;

    if (!fs::is_directory(dbPath)) {
      throw runtime_error("Config Directory Not Found.");
    }

    vector<string> json_paths = glob(dbPath, basic_regex("json"));

    for(const fs::path &p : json_paths) {
      ifstream i(p);
      json conf;
      i >> conf;
      for (auto it = conf.begin(); it != conf.end(); ++it) {
        megaJson[it.key()] = it.value();
      }
    }
  }

  json Config::operator[](json::json_pointer pointer) {
    return megaJson[pointer];
  }

  json Config::operator[](string pointer) {
    return megaJson[pointer];
  }

  json Config::evaluateJson(json eval_json, bool merge) {

    vector<json::json_pointer> json_to_eval = SpiceQL::findKeyInJson(eval_json, "kernels", true);

    for (auto pointer:json_to_eval) {
      vector<string> res = getPathsFromRegex(getDataDirectory(), eval_json[pointer]);
      eval_json[pointer] = res;
    }

    if (merge) {
      megaJson.merge_patch(eval_json);
    }

    return eval_json;
  }

  json Config::evaluateJson(json::json_pointer pointer, bool merge) {
    json eval_json;
    eval_json[pointer] = megaJson[pointer];

    return evaluateJson(eval_json, merge);
  }

  json Config::evaluateJson() {
    return evaluateJson(megaJson, true);
  }

  json Config::getGlobalJson() {
    return megaJson;
  }

  vector<json::json_pointer> Config::findKeyInJson(string key, bool recursive) {
    return SpiceQL::findKeyInJson(megaJson, key, recursive);
  }
}
