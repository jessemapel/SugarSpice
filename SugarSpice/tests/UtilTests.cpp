#include <gtest/gtest.h>

#include "utils.h"
#include "Fixtures.h"


TEST(UtilTests, GetKernelType) { 
   std::cout << getKernelType("/usgs/cpkgs/isis3/isis_data/lro/kernels/fk/lro_frames_2007322_v01.tf") << std::endl;
   EXPECT_TRUE(0); 
}


TEST(UtilTests, GetFrameCode) { 
   std::cout << getFrameCode("MERCURY") << std::endl;
   EXPECT_TRUE(0); 
}
