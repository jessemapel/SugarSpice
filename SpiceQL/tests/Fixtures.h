#pragma once

#include <HippoMocks/hippomocks.h>

#include "gtest/gtest.h"
#include <ghc/fs_std.hpp>

#include "spice_types.h"

using namespace std;
using namespace SpiceQL;

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
    KernelPool &pool = KernelPool::getInstance();

    string root;
    string lskPath;
    string sclkPath;
    string ckPath1;
    string ckPath2;
    string spkPath1;
    string spkPath2;
    string ikPath1;
    string ikPath2;
    string fkPath;

    nlohmann::json conf;

    void SetUp() override;
    void TearDown() override;
};

