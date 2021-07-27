#include <gtest/gtest.h>

#include "utils.h"
#include "Fixtures.h"
#include "spice_types.h"
#include "SpiceUsr.h"

using namespace SugarSpice;

TEST(UtilTests, GetKernelType) { 
}


TEST(UtilTests, GetFrameCode) { 
}


TEST(UtilTests, GetFrameName) {  
}

TEST(UtilTests, getTargetState) {
  unique_ptr<Kernel> ka(new Kernel("/usgs/cpkgs/isis3/data/messenger/kernels/spk/msgr_20040803_20150328_od397sc_0.bsp"));
  unique_ptr<Kernel> kb(new Kernel("/usgs/cpkgs/isis3/data/messenger/kernels/ck/msgr_1504_v01.bc"));  // 482828147.5645907 is a good et for this

  // double lt;
  // double state[6];
  targetState ts = getTargetState(482828147.5645907, "Mercury", "Messenger");
  

  cout << "lt: " << ts.lt << endl;
  cout << "state: " << ts.starg[0] << " " << ts.starg[1] <<  " " << ts.starg[2] << " " <<  ts.starg[3] << " " <<  ts.starg[4] << " " << ts.starg[5] << std::endl;

  EXPECT_NEAR(ts.lt, 0.0320471, 0.00001);
  EXPECT_NEAR(ts.starg[0], 8954.19, 0.01);
}

TEST(UtilTests, getTargetOrientation) {
  // unique_ptr<Kernel> ka(new Kernel("/usgs/cpkgs/isis3/data/messenger/kernels/spk/msgr_20040803_20150328_od397sc_0.bsp"));
  // unique_ptr<Kernel> kc(new Kernel("/usgs/cpkgs/isis3/data/messenger/kernels/sclk/messenger_2548.tsc"));
  // unique_ptr<Kernel> kd(new Kernel("/usgs/cpkgs/isis3/data/base/kernels/lsk/naif0012.tls"));
  // unique_ptr<Kernel> ke(new Kernel("/usgs/cpkgs/isis3/data/messenger/kernels/ck/msgr_1504_v01.bc"));  // 482828147.5645907 is a good et for this
  
  furnsh_c("/usgs/cpkgs/isis3/data/messenger/kernels/ck/msgr_0508_v04.bc");
  furnsh_c("/usgs/cpkgs/isis3/data/messenger/kernels/ck/msgr_mdis_sc050727_100302_sub_v1.bc");
  furnsh_c("/usgs/cpkgs/isis3/data/messenger/kernels/ck/msgr_mdis_gm040819_150430v1.bc");
  furnsh_c("/usgs/cpkgs/isis3/data/messenger/kernels/fk/msgr_v231.tf");
  furnsh_c("/usgs/cpkgs/isis3/data/messenger/kernels/sclk/messenger_2548.tsc");
  furnsh_c("/usgs/cpkgs/isis3/data/base/kernels/lsk/naif0012.tls");
  furnsh_c("/usgs/cpkgs/isis3/data/messenger/kernels/ik/msgr_mdis_v160.ti");
  furnsh_c("/usgs/cpkgs/isis3/data/messenger/kernels/iak/mdisAddendum009.ti");
  furnsh_c("/usgs/cpkgs/isis3/data/messenger/kernels/ck/msgr_0508_v04.bc");
  furnsh_c("/usgs/cpkgs/isis3/data/messenger/kernels/ck/msgr_mdis_sc040812_150430v1.bc"); 
  // furnsh_c("/usgs/cpkgs/isis3/data/messenger/kernels/ck/msgr_mdis_sc050727_100302_sub_v1.bc");
  // furnsh_c("/usgs/cpkgs/isis3/data/messenger/kernels/ck/msgr_mdis_gm040819_150430v1.bc"); // repeat
  furnsh_c("/usgs/cpkgs/isis3/data/messenger/kernels/fk/msgr_v231.tf");
  furnsh_c("/usgs/cpkgs/isis3/data/base/kernels/spk/de405.bsp");
  furnsh_c("/usgs/cpkgs/isis3/data/messenger/kernels/spk/msgr_20040803_20150430_od431sc_2.bsp");
  // furnsh_c("/usgs/cpkgs/isis3/data/messenger/kernels/");
  // furnsh_c("/usgs/cpkgs/isis3/data/messenger/kernels/");


  targetOrientation to = getTargetOrientation(-236, 176293972.98331, 0.0);
  cout << to.etout << endl;
  EXPECT_NEAR(to.etout, 1.0, 0.1);
}

TEST(UtilTests, findKeywords) {
  unique_ptr<Kernel> k(new Kernel("data/msgr_mdis_v010.ti"));

  nlohmann::json res = findKeywords("*");
  EXPECT_EQ(res.at("INS-236810_FOV_SHAPE"), "RECTANGLE");
  EXPECT_EQ(res.at("INS-236800_WAVELENGTH_RANGE")[1], 1040);
  EXPECT_EQ(res.at("INS-236800_IFOV"), 179.6);
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
