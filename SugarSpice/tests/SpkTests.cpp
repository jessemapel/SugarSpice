#include <vector>

#include <gtest/gtest.h>

#include "io.h"

TEST(IOTests, CreateSPKSegment) {
  std::string comment = "This is a comment for \n a test SPK segment";
  int body = 1;
  int center = 1;
  std::string refFrame = "some ref frame";
  double start = 0.0;
  double end = 0.0;
  std::string id = "a test segment";
  int degree = 1;
  int states = 1;
  std::vector<std::vector<double>> pos = {{1.0, 2.0, 3.0}};
  std::vector<std::vector<double>> vel = {{0.1, 0.2, 0.3}};
  std::vector<double> et = {0.0};

  spkSegment seg(comment, body, center, refFrame, start, end, id, degree, states,
                 pos, vel, et);
  EXPECT_EQ(seg.getComment(), comment);

  // Add other member tests

}

TEST(IOTests, WriteSPKSegment) {

  auto tmp_dir = fs::temp_directory_path();
  fs::path tpath;
  tpath = tmp_dir / "test_spk.bsp";

  std::string comment = "This is a comment for \n a test SPK segment";
  int body = 1;
  int center = 1;
  std::string refFrame = "J2000";
  double start = 0.0;
  double end = 0.0;
  std::string id = "a test segment";
  int degree = 1;
  int states = 1;
  std::vector<std::vector<double>> pos = {{1.0, 2.0, 3.0}, {4.0, 5.0, 6.0}};
  std::vector<std::vector<double>> vel = {{0.1, 0.2, 0.3}, {0.4, 0.5, 0.6}};
  std::vector<double> et = {0.0, 1.0};

  std::vector<spkSegment> segments;
  segments.push_back(spkSegment(comment, body, center, refFrame, start, end, id, degree, states,
                 pos, vel, et));

  writeSpk (tpath, "", segments);

}
