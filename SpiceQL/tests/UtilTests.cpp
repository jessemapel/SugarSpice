#include <gtest/gtest.h>

#include "utils.h"
#include "Fixtures.h"
#include "spice_types.h"
#include "SpiceUsr.h"

using namespace SpiceQL;

TEST(UtilTests, GetKernelType) { 
}


TEST(UtilTests, GetFrameCode) { 
}


TEST(UtilTests, GetFrameName) {  
}

/*
 * needs local test data
TEST(UtilTests, getTargetState) {
  unique_ptr<Kernel> spk(new Kernel("/usgs/cpkgs/isis3/data/messenger/kernels/spk/msgr_20040803_20150328_od397sc_0.bsp"));
  unique_ptr<Kernel> ck(new Kernel("/usgs/cpkgs/isis3/data/messenger/kernels/ck/msgr_1504_v01.bc"));  // 482828147.5645907 is a good et for this

  targetState ts = getTargetState(482828147.5645907, "Mercury", "Messenger");
  

  EXPECT_NEAR(ts.lt, 0.0320471, 0.00001);
  EXPECT_NEAR(ts.starg[0], 8954.19, 0.01);
  EXPECT_NEAR(ts.starg[1], -3025.57, 0.01);
  EXPECT_NEAR(ts.starg[2], 1723.98, 0.01);
  EXPECT_NEAR(ts.starg[3], 0.583899, 1.0e-5);
  EXPECT_NEAR(ts.starg[4], -0.540947, 1.0e-5);
  EXPECT_NEAR(ts.starg[5], 1.08804, 1.0e-5);
}
*/

/*
 * needs local test data
TEST(UtilTests, getTargetOrientation) {
  furnsh_c("/usgs/cpkgs/isis3/data/messenger/kernels/ck/msgr_0508_v04.bc");   // 176293972.98331 is a good time
  furnsh_c("/usgs/cpkgs/isis3/data/messenger/kernels/ck/msgr_mdis_sc050727_100302_sub_v1.bc");
  furnsh_c("/usgs/cpkgs/isis3/data/messenger/kernels/ck/msgr_mdis_gm040819_150430v1.bc");
  furnsh_c("/usgs/cpkgs/isis3/data/messenger/kernels/fk/msgr_v231.tf");
  furnsh_c("/usgs/cpkgs/isis3/data/messenger/kernels/sclk/messenger_2548.tsc");
  furnsh_c("/usgs/cpkgs/isis3/data/base/kernels/lsk/naif0012.tls");
  furnsh_c("/usgs/cpkgs/isis3/data/messenger/kernels/ik/msgr_mdis_v160.ti");
  furnsh_c("/usgs/cpkgs/isis3/data/messenger/kernels/iak/mdisAddendum009.ti");
  furnsh_c("/usgs/cpkgs/isis3/data/messenger/kernels/ck/msgr_0508_v04.bc");
  furnsh_c("/usgs/cpkgs/isis3/data/messenger/kernels/ck/msgr_mdis_sc040812_150430v1.bc"); 
  furnsh_c("/usgs/cpkgs/isis3/data/messenger/kernels/fk/msgr_v231.tf");
  furnsh_c("/usgs/cpkgs/isis3/data/base/kernels/spk/de405.bsp");
  furnsh_c("/usgs/cpkgs/isis3/data/messenger/kernels/spk/msgr_20040803_20150430_od431sc_2.bsp");


  targetOrientation orientation = getTargetOrientation(176293972.98331, -236000);
  if( !orientation.av ) {
    FAIL();
  }

  double av_truth[3] = {-2.83036e-05, -4.11081e-05, -6.51215e-05};
  double quat_truth[4] = {0.716887, 0.0942421, -0.57821, -0.377975};

  for(int i = 0; i < 4; i++) {
    if(i < 3)  EXPECT_NEAR(orientation.av.value()[i], av_truth[i], 1.0e-5);
    EXPECT_NEAR(orientation.quat[i], quat_truth[i], 1.0e-5);
  }
  
}
*/

TEST(UtilTests, findKeywords) {
  unique_ptr<Kernel> k(new Kernel("data/msgr_mdis_v010.ti"));

  std::cout << KernelPool::refCounts.at("data/msgr_mdis_v010.ti") << std::endl;

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
