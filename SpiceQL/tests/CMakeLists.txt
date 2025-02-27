cmake_minimum_required(VERSION 3.10)

set(SPICEQL_TEST_DIRECTORY ${CMAKE_SOURCE_DIR}/SpiceQL/tests/)

# collect all of the test sources
set (SPICEQL_TEST_SOURCE ${SPICEQL_TEST_DIRECTORY}/Fixtures.cpp
                            ${SPICEQL_TEST_DIRECTORY}/UtilTests.cpp
                            ${SPICEQL_TEST_DIRECTORY}/QueryTests.cpp
                            ${SPICEQL_TEST_DIRECTORY}/IoTests.cpp
                            ${SPICEQL_TEST_DIRECTORY}/KernelTests.cpp
                            ${SPICEQL_TEST_DIRECTORY}/FunctionalTestsSpiceQueries.cpp)

# setup test executable
add_executable(runSpiceQLTests TestMain.cpp ${SPICEQL_TEST_SOURCE})
target_link_libraries(runSpiceQLTests
                      PRIVATE
                      SpiceQL
                      CSpice::cspice
                      gtest
                      gmock
                      Threads::Threads
                      )


target_include_directories(runSpiceQLTests PRIVATE ${CMAKE_SOURCE_DIR}/submodules/hippomocks/)

gtest_discover_tests(runSpiceQLTests WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/SpiceQL/tests/)
