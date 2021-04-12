#include <gtest/gtest.h>

#include "Fixtures.h"

#include "query.h"

TEST_F(IsisDataDirectories, UnitTestSearchMissionKernels) { 
   nlohmann::json res = searchMissionKernels(tempDir, "mess");
   std::cout << res << std::endl;

   ASSERT_EQ();

}