#include <fstream>

#include <gtest/gtest.h>

#include "Fixtures.h"

#include "query.h"
#include "utils.h"

using namespace std;


TEST(QueryTests, UnitTestGetLatestKernel) { 
  vector<fs::path> kernels = {
    "iak.0001.ti",
    "iak.0003.ti",
    "different/place/iak.0002.ti",
    "test/iak.0004.ti"
  };

  EXPECT_EQ(getLatestKernel(kernels),  "test/iak.0004.ti");
}


TEST(QueryTests, UnitTestGetLatestKernelError) { 
  vector<fs::path> kernels = {
    "iak.0001.ti",
    "iak.0003.ti",
    "different/place/iak.0002.ti",
    "test/iak.4.ti",
    // different extension means different filetype and therefore error
    "test/error.tf" 
  };

  try { 
    getLatestKernel(kernels);
    FAIL() << "expected invalid argument error"; 
  }
  catch(invalid_argument &e) {
    SUCCEED();
  }
}


TEST_F(KernelDataDirectories, FunctionalTestSearchMissionKernelsAllMess) {
  fs::path dbPath = getMissionConfigFile("mess");

  ifstream i(dbPath);
  nlohmann::json conf;
  i >> conf;

  MockRepository mocks;
  mocks.OnCallFunc(ls).Return(paths);

  nlohmann::json res = searchMissionKernels("/isis_data/", conf);

  ASSERT_EQ(res["mdis"]["ck"]["reconstructed"].size(), 4);
  ASSERT_EQ(res["mdis"]["ck"]["smithed"].size(), 4);
  ASSERT_EQ(res["mdis"]["ck"]["deps"]["sclk"].size(), 2);
  ASSERT_EQ(res["mdis"]["ck"]["deps"]["lsk"].size(), 2);
  ASSERT_EQ(res["mdis"]["ck"]["deps"]["objs"].size(), 2);
  ASSERT_EQ(res["mdis"]["spk"]["reconstructed"].size(), 2);
  ASSERT_EQ(res["mdis"]["spk"]["deps"]["lsk"].size(), 2);
  ASSERT_EQ(res["mdis"]["tspk"]["na"].size(), 1);
  ASSERT_EQ(res["mdis"]["fk"].size(), 2);
  ASSERT_EQ(res["mdis"]["ik"].size(), 2);
  ASSERT_EQ(res["mdis"]["iak"].size(), 2);
  ASSERT_EQ(res["mdis"]["pck"].size(), 2);

  ASSERT_EQ(res["mdis_att"]["ck"]["reconstructed"].size(), 4);
  ASSERT_EQ(res["mdis_att"]["ck"]["deps"]["sclk"].size(), 2);
  ASSERT_EQ(res["mdis_att"]["ck"]["deps"]["lsk"].size(), 2);
  ASSERT_EQ(res["mdis_att"]["ck"]["deps"]["objs"].size(), 0);

  ASSERT_EQ(res["mess"]["ck"]["reconstructed"].size(), 5);
  ASSERT_EQ(res["mess"]["ck"]["deps"]["sclk"].size(), 2);
  ASSERT_EQ(res["mess"]["ck"]["deps"]["lsk"].size(), 2);
  ASSERT_EQ(res["mess"]["ck"]["deps"]["objs"].size(), 0);
}


TEST_F(KernelDataDirectories, FunctionalTestSearchMissionKernelsClem1) {
  fs::path dbPath = getMissionConfigFile("clem1");

  ifstream i(dbPath);
  nlohmann::json conf;
  i >> conf;

  MockRepository mocks;
  mocks.OnCallFunc(ls).Return(paths);

  nlohmann::json res = searchMissionKernels("/isis_data/", conf);

  ASSERT_EQ(res["clem"]["ck"]["reconstructed"].size(), 4);
  ASSERT_EQ(res["clem"]["ck"]["smithed"].size(), 1);
  ASSERT_EQ(res["clem"]["ck"]["deps"]["sclk"].size(), 2);
  ASSERT_EQ(res["clem"]["ck"]["deps"]["lsk"].size(), 2);
  ASSERT_EQ(res["clem"]["ck"]["deps"]["objs"].size(), 0);
  ASSERT_EQ(res["clem"]["spk"]["reconstructed"].size(), 2);
  ASSERT_EQ(res["clem"]["spk"]["deps"]["lsk"].size(), 2);
  ASSERT_EQ(res["clem"]["fk"].size(), 1);

  ASSERT_EQ(res["UVVIS"]["ik"].size(), 1);
  ASSERT_EQ(res["UVVIS"]["iak"].size(), 2);

  ASSERT_EQ(res["UVVIS"]["iak"].size(), 2);
}
 

TEST(he, things) { 
  // nlohmann::json conf = getMissionConfig("mess");
  // nlohmann::json kernels = searchMissionKernels("/data/spice/", conf);
  // std::cout << kernels << std::endl;
  // kernels = searchMissionKernels();

  std::cout << utcToEt("1996-12-18T12:28:28") << std::endl;

}