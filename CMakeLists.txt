cmake_minimum_required(VERSION 3.10)
project(SugarSpice VERSION 0.0.1 DESCRIPTION "Syntax Sugar for cspice")

set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_CXX_COMPILER "$ENV{PREFIX}/bin/clang")

# Set a default build type if none was specified
set(default_build_type "Release")
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
  set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE
      STRING "Choose the type of build." FORCE)
endif()

add_subdirectory("submodules/gularkfilesystem")

find_package(CSpice REQUIRED)

add_executable(SugarSpice
               src/SugarSpice.cpp)

set_target_properties(SugarSpice PROPERTIES
    VERSION ${PROJECT_VERSION})

target_include_directories(SugarSpice
                           PUBLIC
                           ${SUGARSPICE_INCLUDE_DIR}
)

target_link_libraries(SugarSpice ghc_filesystem CSpice::cspice)

install(TARGETS SugarSpice LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(DIRECTORY ${SUGARSPICE_INCLUDE_DIR} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# Optional build tests
option (USGSCSM_BUILD_TESTS "Build tests" OFF)

if(USGSCSM_BUILD_TESTS)

  include(GoogleTest)
  include(cmake/gtest.cmake)

  # Setup for GoogleTest
  find_package (Threads)

  target_link_libraries(usgscsm
                        gtest ${CMAKE_THREAD_LIBS_INIT})
  include(CTest)
  enable_testing()
  add_subdirectory(tests)
endif()
