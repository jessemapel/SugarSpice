#pragma once
#include <ghc/fs_std.hpp>

#include "gtest/gtest.h"


class TempTestingFiles : public ::testing::Test {
  protected:
    fs::path tempDir;

    void SetUp() override;
    void TearDown() override; 
};

class IsisDataDirectories : public TempTestingFiles {
  protected:

    void SetUp() override;
    void TearDown() override;
    void LoadDirectory(std::string name);
};



