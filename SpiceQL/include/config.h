#pragma once

#include <iostream>
#include <regex>
#include <optional>

#include <nlohmann/json.hpp>

#include "utils.h"

/**
 * @namespace SpiceQL types
 *
 */
namespace SpiceQL {

  class Config {
    public:

      Config();

      nlohmann::json operator[](nlohmann::json::json_pointer pointer);

      nlohmann::json operator[](std::string pointer);

      nlohmann::json evaluateJson(nlohmann::json eval_json, bool merge=false);

      nlohmann::json evaluateJson(nlohmann::json::json_pointer pointer, bool merge=false);

      nlohmann::json evaluateJson();

      nlohmann::json getGlobalJson();

      std::vector<nlohmann::json::json_pointer> findKeyInJson(std::string key, bool recursive);


    private:

      nlohmann::json megaJson;
  };

}
