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

    vector<fs::path> paths; 
    
    void SetUp() override;
    void TearDown() override;
};


class KernelSet : public TempTestingFiles { 
  protected: 

    std::vector<fs::path> cks; 
    std::vector<fs::path> spks; 
    
    void SetUp() override; 
    void TearDown() override; 
};

