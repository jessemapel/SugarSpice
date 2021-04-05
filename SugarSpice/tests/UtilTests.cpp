#include <gtest/gtest.h>

#include "utils.h"
#include "Fixtures.h"
#include "spice_types.h"

TEST(UtilTests, GetKernelType) { 
   std::cout << getKernelType("/usgs/cpkgs/isis3/isis_data/lro/kernels/fk/lro_frames_2007322_v01.tf") << std::endl;
   EXPECT_TRUE(0); 
}


TEST(UtilTests, GetFrameCode) { 
   std::cout << Kernel::translateFrame("MERCURY") << std::endl;
   EXPECT_TRUE(0); 
}


TEST(UtilTests, GetFrameName) { 
   Kernel::translateFrame(199);
   EXPECT_TRUE(0); 
}

TEST(UtilTests, GetKernelDirs) { 
    getDbFile("lro"); 
    getKernelDir("/data/spice/", "", "", Kernel::Type::CK);

}