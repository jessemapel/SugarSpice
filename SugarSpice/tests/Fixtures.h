#pragma once
#include <ghc/fs_std.hpp>

#include <HippoMocks/hippomocks.h>

#include "gtest/gtest.h"

using namespace std; 

class TempTestingFiles : public ::testing::Test {
  protected:
    fs::path tempDir;

    void SetUp() override;
    void TearDown() override; 
};

class KernelDataDirectories : public ::testing::Test {
  protected:

    vector<fs::path> paths; 
    
    void SetUp() override;
    void TearDown() override;
};



