#include <gtest/gtest.h>
#include <vector>

#include "Fixtures.h"
#include "io.h"
#include "utils.h"

using namespace SugarSpice;


TEST_F(TempTestingFiles, UnitTestWriteCkTest) {
  fs::path path;
  path = tempDir / "test_ck.bsp";

  fs::path lskPath = fs::path("data") / "naif0012.tls"; 
  fs::path sclkPath = fs::path("data") / "lro_clkcor_2020184_v00.tsc";
  
  std::vector<std::vector<double>> orientations = {{0.2886751, 0.2886751, 0.5773503, 0.7071068 }, {0.4082483, 0.4082483, 0.8164966, 0 }};
  std::vector<std::vector<double>> av = {{1,1,1}, {1,2,3}};
  std::vector<double> times = {1, 2};
  int bodyCode = -236000; 
  std::string referenceFrame = "j2000";
  std::string segmentId = "CKCKCK";

  writeCk(path, orientations, times, bodyCode, referenceFrame, segmentId, sclkPath, lskPath, av);
}


TEST(IOTests, CreateSPKSegmentTest) {
  std::string comment = "This is a comment for \n a test SPK segment";
  int body = 1;
  int center = 1;
  std::string refFrame = "B1950";
  std::string id = "a test segment";
  int degree = 1;
  std::vector<std::vector<double>> pos = {{1.0, 2.0, 3.0}};
  std::vector<std::vector<double>> vel = {{0.1, 0.2, 0.3}};
  std::vector<double> et = {0.0};

  SpkSegment seg(pos, et, body, center, refFrame, id, degree, vel, comment);
  EXPECT_EQ(seg.comment, comment);

  // Add other member tests
}


TEST_F(TempTestingFiles, WriteSPKSegmentTest) {
  fs::path tpath;
  tpath = tempDir / "test_spk.bsp";

  std::string comment = "This is a comment for \n a test SPK segment";
  int body = 1;
  int center = 2;
  std::string refFrame = "J2000";
  std::string id = "a test segment";
  int degree = 1;
  std::vector<std::vector<double>> pos = {{1.0, 2.0, 3.0}, {4.0, 5.0, 6.0}};
  std::vector<std::vector<double>> vel = {{0.1, 0.2, 0.3}, {0.4, 0.5, 0.6}};
  std::vector<double> et = {0.0, 1.0};

  std::vector<SpkSegment> segments;
  segments.push_back(SpkSegment(pos, et, body, center, refFrame, id, degree, vel, comment));

  writeSpk (tpath, segments);

  // TODO: Once we can read add a read and tests here
}


TEST_F(TempTestingFiles, writeTextKernelTest) { 
  fs::path tpath = tempDir / "test_ik.ti";

  nlohmann::json j = {
    {"pi", 3.141},
    {"happy", true},
    {"name", "Niels"},
    {"nothing", nullptr},
    {"array", {1, 2, 3, 5.0}}
    }; 

  writeTextKernel(tpath, "ik", j, "This is a IK kernel");

  // furnsh the new kernel
  StackKernel k(new Kernel(tpath));

  // get all keys in the variable pool
  nlohmann::json j2 = findKeywords("*");

  // diff should return an empty json array as input and output should be exactly the same
  ASSERT_EQ(nlohmann::json::diff(j, j2), nlohmann::json::array());
}


