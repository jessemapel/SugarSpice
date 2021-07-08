#include <gtest/gtest.h>
#include <CSpice/SpiceUsr.h>

#include "utils.h"
#include "Fixtures.h"
#include "spice_types.h"

using namespace SugarSpice;

TEST(UtilTests, GetKernelType) { 
}


TEST(UtilTests, GetFrameCode) { 
}


TEST(UtilTests, GetFrameName) {  
}

TEST(UtilTests, findKeywords) {
  furnsh_c("data/msgr_mdis_v010.ti");

  nlohmann::json res = findKeywords("*");

  EXPECT_EQ(res.at("INS-236810_FOV_SHAPE")[0], "RECTANGLE");
  EXPECT_EQ(res.at("INS-236800_WAVELENGTH_RANGE")[1], 1040);
  EXPECT_EQ(res.at("INS-236800_IFOV")[0], 179.6);
}

TEST(UtilTests, findKeyInJson) {
  nlohmann::ordered_json j = R"(
    {
      "me" : "test",
      "l1a" : {
        "l2a" : 1,
        "me" : 2, 
        "l2b" : {
          "l3a" : "yay", 
          "me" : 1
        }
      }
    })"_json;

  std::vector<nlohmann::json::json_pointer> res = findKeyInJson(j, "me", true);

  EXPECT_EQ(res.at(0).to_string(), "/l1a/l2b/me");
  EXPECT_EQ(res.at(1).to_string(), "/l1a/me");
  EXPECT_EQ(res.at(2).to_string(), "/me");
}
