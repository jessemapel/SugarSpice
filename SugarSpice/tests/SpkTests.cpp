#include <vector>

#include <gtest/gtest.h>

#include "Fixtures.h"
#include "io.h"

using namespace SugarSpice;

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

  SpkSegment seg(comment, body, center, refFrame, id, degree, pos, vel, et);
  EXPECT_EQ(seg.getComment(), comment);

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
  segments.push_back(SpkSegment(comment, body, center, refFrame, id, degree, pos, vel, et));

  writeSpk (tpath, "", segments);

  // TODO: Once we can read add a read and tests here
}
