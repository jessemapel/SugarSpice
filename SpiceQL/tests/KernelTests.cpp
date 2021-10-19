#include <gtest/gtest.h>
#include <fmt/format.h>

#include "utils.h"
#include "Fixtures.h"
#include "spice_types.h"
#include "query.h"

#include "SpiceUsr.h"

using namespace SpiceQL;


TEST_F(LroKernelSet, UnitTestStackedKernelConstructorDestructor) { 
  int nkernels;

  // This should create local kernels that get unfurnished when the stack is popped 
  {
    Kernel k(lskPath);
    
    // should match what spice counts
    ktotal_c("text", &nkernels);

    ASSERT_EQ(nkernels, 1);
    ASSERT_EQ(KernelPool::refCounts.at(lskPath), 1);
  }

  ktotal_c("text", &nkernels);
  ASSERT_EQ(nkernels, 0); 
  ASSERT_EQ(KernelPool::getRefCount(lskPath), 0);
}


TEST_F(LroKernelSet, UnitTestStackedKernelCopyConstructor) { 
  int nkernels;

  // This should create local kernels that get unfurnished when the stack is popped 
  {
    Kernel k(lskPath);
    Kernel k2 = k; 
    Kernel k3(k2);

    // should match what spice counts
    ktotal_c("text", &nkernels);

    ASSERT_EQ(nkernels, 1);
    ASSERT_EQ(KernelPool::refCounts.at(lskPath), 3);
  }

  ktotal_c("text", &nkernels);
  ASSERT_EQ(nkernels, 0); 
  ASSERT_EQ(KernelPool::getRefCount(lskPath), 0);
}


TEST_F(LroKernelSet, UnitTestStackedKernelSetConstructorDestructor) {
  // Complete a kernel search
  setenv("SPICEROOT", tempDir.c_str(), true);

  nlohmann::json conf = getMissionConfig("lro");

  // load all available kernels
  nlohmann::json kernels = searchMissionKernels(root, conf);
  
  // do a time query
  kernels = searchMissionKernels(kernels, {110000000, 120000001}, false);
  
  // get only latest versions
  kernels = getLatestKernels(kernels);
  
  // all the kernels in the group are now furnished. 
  KernelSet ks(kernels);

  int nkernels;
  
  // load kernels in a closed call stack
  {
    KernelSet ks(kernels);
    // should match what spice counts
    ktotal_c("text", &nkernels);
    ASSERT_EQ(nkernels, 3);
    ktotal_c("ck", &nkernels);
    ASSERT_EQ(nkernels, 1);
    ktotal_c("spk", &nkernels);
    ASSERT_EQ(nkernels, 1);
    
    // 5 because LSK is not being loaded (yet)
    ASSERT_EQ(KernelPool::refCounts.size(), 5);
    ASSERT_EQ(KernelPool::getRefCount(fkPath), 1); 
    ASSERT_EQ(KernelPool::getRefCount(ckPath1), 1); 
    ASSERT_EQ(KernelPool::getRefCount(spkPath1), 1); 
    ASSERT_EQ(KernelPool::getRefCount(sclkPath), 1); 
    ASSERT_EQ(KernelPool::getRefCount(ikPath2), 1); 
  }

  // All kernels should be unfurnished
  ktotal_c("text", &nkernels);
  ASSERT_EQ(nkernels, 0);
  ktotal_c("ck", &nkernels);
  ASSERT_EQ(nkernels, 0);
  ktotal_c("spk", &nkernels);
  ASSERT_EQ(nkernels, 0);
  
  ASSERT_EQ(KernelPool::refCounts.size(), 0);
  ASSERT_EQ(KernelPool::getRefCount(fkPath), 0); 
  ASSERT_EQ(KernelPool::getRefCount(ckPath1), 0); 
  ASSERT_EQ(KernelPool::getRefCount(spkPath1), 0); 
  ASSERT_EQ(KernelPool::getRefCount(sclkPath), 0); 
  ASSERT_EQ(KernelPool::getRefCount(ikPath2), 0); 
}


TEST_F(LroKernelSet, UnitTestStackedKernelPoolGetLoadedKernels) {
  // Complete a kernel search
  setenv("SPICEROOT", tempDir.c_str(), true);

  nlohmann::json conf = getMissionConfig("lro");

  // load all available kernels
  nlohmann::json kernels = searchMissionKernels(root, conf);
  
  // do a time query
  kernels = searchMissionKernels(kernels, {110000000, 120000001}, false);
  kernels = getLatestKernels(kernels);

  KernelSet k(kernels);

  std::vector<string> kv = KernelPool::getLoadedKernels();
  ASSERT_EQ(kv.size(), 5);
  ASSERT_TRUE(std::find(kv.begin(), kv.end(), fkPath) != kv.end()); 
  ASSERT_TRUE(std::find(kv.begin(), kv.end(), ckPath1) != kv.end());
  ASSERT_TRUE(std::find(kv.begin(), kv.end(), spkPath1) != kv.end());
  ASSERT_TRUE(std::find(kv.begin(), kv.end(), sclkPath) != kv.end());
  ASSERT_TRUE(std::find(kv.begin(), kv.end(), ikPath2) != kv.end());
}


TEST_F(LroKernelSet, UnitTestLoadTimeKernels) {
  setenv("SPICEROOT", tempDir.c_str(), true);

  std::shared_ptr<KernelSet> k = loadTimeKernels();

  std::vector<string> kv = KernelPool::getLoadedKernels();
  ASSERT_EQ(kv.at(0), k->kernels.at("/moc/sclk/kernels/0"_json_pointer));
  ASSERT_EQ(kv.at(1), k->kernels.at("/base/lsk/kernels"_json_pointer)); 
}