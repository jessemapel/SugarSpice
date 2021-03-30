cmake_minimum_required(VERSION 3.10)

set(SUGARSPICE_TEST_DIRECTORY ${CMAKE_SOURCE_DIR}/SugarSpice/tests/)

# collect all of the test sources
set (SUGARSPICE_TEST_SOURCE ${SUGARSPICE_TEST_DIRECTORY}/CKSearchTests.cpp
                            ${SUGARSPICE_TEST_DIRECTORY}/UtilTests.cpp)

# setup test executable
add_executable(runSugarSpiceTests TestMain.cpp ${SUGARSPICE_TEST_SOURCE})
target_link_libraries(runSugarSpiceTests
                      PRIVATE
                      SugarSpice
                      gtest
                      gmock
                      Threads::Threads
                      )


gtest_discover_tests(runSugarSpiceTests WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/SugarSpice/tests/)
