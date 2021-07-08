#include <fstream>

#include <gtest/gtest.h>

#include "Fixtures.h"

#include "query.h"
#include "utils.h"

using namespace std;
using namespace SugarSpice;


TEST(QueryTests, UnitTestGetLatestKernel) { 
  vector<string> kernels = {
    "iak.0001.ti",
    "iak.0003.ti",
    "different/place/iak.0002.ti",
    "test/iak.0004.ti"
  };

  EXPECT_EQ(getLatestKernel(kernels),  "test/iak.0004.ti");
}


TEST(QueryTests, UnitTestGetLatestKernelError) { 
  vector<string> kernels = {
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
  
  ASSERT_EQ(res["mdis"]["ck"]["reconstructed"]["kernels"].size(), 4);
  ASSERT_EQ(res["mdis"]["ck"]["smithed"]["kernels"].size(), 4);
  ASSERT_EQ(res["mdis"]["ck"]["deps"]["sclk"].size(), 2);
  ASSERT_EQ(res["mdis"]["ck"]["deps"]["objs"].size(), 3);
  ASSERT_EQ(res["mdis"]["spk"]["reconstructed"]["kernels"].size(), 2);
  ASSERT_EQ(res["mdis"]["tspk"]["kernels"].size(), 1);
  ASSERT_EQ(res["mdis"]["fk"]["kernels"].size(), 2);
  ASSERT_EQ(res["mdis"]["ik"]["kernels"].size(), 2);
  ASSERT_EQ(res["mdis"]["iak"]["kernels"].size(), 2);
  ASSERT_EQ(res["mdis"]["pck"]["na"]["kernels"].size(), 2);

  ASSERT_EQ(res["mdis_att"]["ck"]["reconstructed"]["kernels"].size(), 4);
  ASSERT_EQ(res["mdis_att"]["ck"]["deps"]["sclk"].size(), 2);
  ASSERT_EQ(res["mdis_att"]["ck"]["deps"]["objs"].size(), 0);

  ASSERT_EQ(res["mess"]["ck"]["reconstructed"]["kernels"].size(), 5);
  ASSERT_EQ(res["mess"]["ck"]["deps"]["sclk"].size(), 2);
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

  ASSERT_EQ(res["clem"]["ck"]["reconstructed"]["kernels"].size(), 4);
  ASSERT_EQ(res["clem"]["ck"]["smithed"]["kernels"].size(), 1);
  ASSERT_EQ(res["clem"]["ck"]["deps"]["sclk"].size(), 2);
  ASSERT_EQ(res["clem"]["ck"]["deps"]["objs"].size(), 1);
  ASSERT_EQ(res["clem"]["spk"]["reconstructed"]["kernels"].size(), 2);
  ASSERT_EQ(res["clem"]["fk"]["kernels"].size(), 1);

  ASSERT_EQ(res["uvvis"]["ik"]["kernels"].size(), 1);
  ASSERT_EQ(res["uvvis"]["iak"]["kernels"].size(), 2);

  ASSERT_EQ(res["uvvis"]["iak"]["kernels"].size(), 2);
}
 

TEST_F(KernelDataDirectories, FunctionalTestSearchMissionKernelsGalileo) {
  fs::path dbPath = getMissionConfigFile("galileo");

  ifstream i(dbPath);
  nlohmann::json conf;
  i >> conf;

  MockRepository mocks;
  mocks.OnCallFunc(ls).Return(paths);

  nlohmann::json res = searchMissionKernels("/isis_data/", conf);

  ASSERT_EQ(res["galileo"]["ck"]["reconstructed"]["kernels"].size(), 4);
  ASSERT_EQ(res["galileo"]["ck"]["reconstructed"]["deps"].size(), 0);
  ASSERT_EQ(res["galileo"]["ck"]["smithed"]["kernels"].size(), 3);
  ASSERT_EQ(res["galileo"]["ck"]["smithed"]["deps"]["objs"].size(), 1);
  ASSERT_EQ(res["galileo"]["ck"]["deps"]["sclk"].size(), 1);
  ASSERT_EQ(res["galileo"]["ck"]["deps"]["objs"].size(), 0);
  ASSERT_EQ(res["galileo"]["spk"]["reconstructed"]["kernels"].size(), 2);
  ASSERT_EQ(res["galileo"]["iak"]["kernels"].size(), 1);
  ASSERT_EQ(res["galileo"]["pck"]["smithed"]["kernels"].size(), 2);
  ASSERT_EQ(res["galileo"]["pck"]["smithed"]["deps"].size(), 0);
  ASSERT_EQ(res["galileo"]["pck"]["na"]["kernels"].size(), 1);
  ASSERT_EQ(res["galileo"]["pck"]["na"]["deps"].size(), 0);
}

