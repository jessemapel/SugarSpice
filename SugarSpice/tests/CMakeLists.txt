cmake_minimum_required(VERSION 3.10)

set(SUGARSPICE_TEST_DIRECTORY ${CMAKE_SOURCE_DIR}/SugarSpice/tests/)

# collect all of the test sources
set (SUGARSPICE_TEST_SOURCE ${SUGARSPICE_TEST_DIRECTORY}/Fixtures.cpp
                            ${SUGARSPICE_TEST_DIRECTORY}/UtilTests.cpp
                            ${SUGARSPICE_TEST_DIRECTORY}/QueryTests.cpp)

# setup test executable
add_executable(runSugarSpiceTests TestMain.cpp ${SUGARSPICE_TEST_SOURCE})
target_link_libraries(runSugarSpiceTests
                      PRIVATE
                      SugarSpice
                      gtest
                      gmock
                      Threads::Threads
                      )


target_include_directories(runSugarSpiceTests PRIVATE ${CMAKE_SOURCE_DIR}/submodules/hippomocks/)

gtest_discover_tests(runSugarSpiceTests WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/SugarSpice/tests/)
