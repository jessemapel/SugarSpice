cmake_minimum_required(VERSION 3.10)
project(SugarSpice VERSION 0.0.1 DESCRIPTION "Syntax Sugar for cspice")

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

set(CMAKE_CXX_STANDARD 20)
# set(CMAKE_CXX_COMPILER "$ENV{PREFIX}/bin/clang")

include(CMakeDependentOption)

# Build options
option (SUGARSPICE_BUILD_LIB "Build the SugarSpice Library" ON)
option (SUGARSPICE_BUILD_DOCS "Build the SugarSpice Docs" ON)
cmake_dependent_option (SUGARSPICE_BUILD_TESTS "Build the SugarSpice Tests" ON SUGARSPICE_BUILD_LIB OFF)

# Variables required by multiple build options
set(SUGARSPICE_BUILD_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/SugarSpice/include/")

#################
# Library Build #
#################

if(SUGARSPICE_BUILD_LIB)

  # Set a default build type if none was specified
  set(default_build_type "Release")
  if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
    set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE
        STRING "Choose the type of build." FORCE)
  endif()

  if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "$ENV{CONDA_PREFIX}")
  endif()
  message(STATUS "Using install prefix: ${CMAKE_INSTALL_PREFIX}")

  set(JSON_BuildTests OFF CACHE INTERNAL "")

  add_subdirectory("submodules/gularkfilesystem")
  add_subdirectory("submodules/json")

  find_package(CSpice REQUIRED)
  find_package(fmt REQUIRED)

  # Library setup
  set(SUGARSPICE_INSTALL_INCLUDE_DIR "include/SugarSpice")
  set(SUGARSPICE_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/SugarSpice/src/utils.cpp
                          ${CMAKE_CURRENT_SOURCE_DIR}/SugarSpice/src/io.cpp
                          ${CMAKE_CURRENT_SOURCE_DIR}/SugarSpice/src/query.cpp
                          ${CMAKE_CURRENT_SOURCE_DIR}/SugarSpice/src/spice_types.cpp)

  set(SUGARSPICE_HEADER_FILES ${SUGARSPICE_BUILD_INCLUDE_DIR}/sugar_spice.h
                              ${SUGARSPICE_BUILD_INCLUDE_DIR}/utils.h
                              ${SUGARSPICE_BUILD_INCLUDE_DIR}/io.h
                              ${SUGARSPICE_BUILD_INCLUDE_DIR}/spice_types.h
                              ${SUGARSPICE_BUILD_INCLUDE_DIR}/query.h)

  set(SUGARSPICE_CONFIG_FILES ${CMAKE_CURRENT_SOURCE_DIR}/SugarSpice/db/clem1.json
                              ${CMAKE_CURRENT_SOURCE_DIR}/SugarSpice/db/galileo.json
                              ${CMAKE_CURRENT_SOURCE_DIR}/SugarSpice/db/lro.json
                              ${CMAKE_CURRENT_SOURCE_DIR}/SugarSpice/db/mess.json)

  add_library(SugarSpice SHARED ${SUGARSPICE_SRC_FILES})

  ADD_DEFINITIONS(-D_SOURCE_PREFIX="${CMAKE_CURRENT_SOURCE_DIR}")


  set_target_properties(SugarSpice PROPERTIES
                                  VERSION ${PROJECT_VERSION}
                                  SOVERSION 0)

  target_include_directories(SugarSpice
                            PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/SugarSpice/include>
                            $<INSTALL_INTERFACE:include>
                            PRIVATE)

  target_link_libraries(SugarSpice
                        PUBLIC
                        ghc_filesystem
                        fmt::fmt-header-only
                        nlohmann_json::nlohmann_json
                        PRIVATE
                        CSpice::cspice
                        )

  install(TARGETS SugarSpice LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
  install(DIRECTORY ${SUGARSPICE_INCLUDE_DIR} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

  # Generate the package config
  configure_file(cmake/config.cmake.in
                ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake
                @ONLY)

  # Install the package config
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake
                DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})

  # Install the headers
  install(FILES ${SUGARSPICE_HEADER_FILES} DESTINATION ${SUGARSPICE_INSTALL_INCLUDE_DIR})

  # Install the json db files
  install(FILES ${SUGARSPICE_CONFIG_FILES} DESTINATION "etc/SugarSpice/db")

  # Install the library
  install(TARGETS SugarSpice nlohmann_json
          EXPORT sugarSpiceTargets
          LIBRARY  DESTINATION ${CMAKE_INSTALL_LIBDIR}
          INCLUDES DESTINATION ${SUGARSPICE_INSTALL_INCLUDE_DIR})

  # Install the target
  install(EXPORT sugarSpiceTargets
          NAMESPACE spice::
          DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})

else()
  message(STATUS "Skipping Library")
endif()

###############
# Tests Build #
###############

if(SUGARSPICE_BUILD_TESTS)
  include(GoogleTest)
  include(cmake/gtest.cmake)
  include(CTest)

  find_package (Threads)
  enable_testing()
  add_subdirectory(SugarSpice/tests)
else()
  message(STATUS "Skipping Tests")
endif()

##############
# Docs Build #
##############

if(SUGARSPICE_BUILD_DOCS)
  add_subdirectory ("docs")
else()
  message(STATUS "Skipping Docs")
endif()
