#pragma once

#include <HippoMocks/hippomocks.h>

#include "gtest/gtest.h"
#include <ghc/fs_std.hpp>

using namespace std;

class TempTestingFiles : public ::testing::Test {
  protected:
    fs::path tempDir;

    void SetUp() override;
    void TearDown() override;
};


class KernelDataDirectories : public ::testing::Test {
  protected:

    vector<string> paths;

    void SetUp() override;
    void TearDown() override;
};


class LroKernelSet : public TempTestingFiles {
  protected:

    string root;

    void SetUp() override;
    void TearDown() override;
};

