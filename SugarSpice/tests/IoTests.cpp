#include <gtest/gtest.h>
#include <vector>

#include "Fixtures.h"
#include "io.h"

using namespace SugarSpice;


TEST_F(TempTestingFiles, UnitTestWriteCk) {
  fs::path path;
  path = tempDir / "test_ck.bsp";

  std::vector<std::vector<double>> orientations = {{0.2886751, 0.2886751, 0.5773503, 0.7071068 }, {0.4082483, 0.4082483, 0.8164966, 0 }};
  std::vector<std::vector<double>> av = {{1,1,1}, {1,2,3}};
  std::vector<double> times = {1, 2};
  int bodyCode = -236000; 
  std::string referenceFrame = "j2000";
  std::string segmentId = "CKCKCK";

  writeCk(path, orientations, times, bodyCode, referenceFrame, segmentId, av);
}


TEST(IOTests, CreateSPKSegment) {
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


TEST_F(TempTestingFiles, WriteSPKSegment) {
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
