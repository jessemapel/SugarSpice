cmake_minimum_required(VERSION 3.14)

# Setup for SWIG
set(CMAKE_SWIG_FLAGS)
find_package(SWIG REQUIRED)
include(UseSWIG)
list(APPEND CMAKE_SWIG_FLAGS "-py3;-DPY3")

# Setup for Python linking
find_package(Python3 REQUIRED COMPONENTS Interpreter Development)

# Setup for wrapper library
set(PYSPICEQL_SOURCES pyspiceql.i
                      config.i
                      io.i
                      query.i
                      spice_types.i
                      utils.i)
set_source_files_properties(${PYSPICEQL_SOURCES} PROPERTIES CPLUSPLUS ON)
swig_add_library(pyspiceql
                 LANGUAGE python
                 SOURCES ${PYSPICEQL_SOURCES}
                 OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/pyspiceql")
swig_link_libraries(pyspiceql SpiceQL Python3::Module)
set_property(TARGET pyspiceql PROPERTY SWIG_USE_TARGET_INCLUDE_DIRECTORIES TRUE)

# Setup for wrapper tests
if(SPICEQL_BUILD_TESTS)
    add_test(NAME python-tests
        COMMAND python -m pytest ${CMAKE_CURRENT_SOURCE_DIR}/tests
        WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/pyspiceql")
else()
    message(STATUS "Skipping Python Bindings Tests")
endif()

# Create the files to install the Python wrapper
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/setup.py.in
               ${CMAKE_CURRENT_BINARY_DIR}/setup.py)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/__init__.py
               ${CMAKE_CURRENT_BINARY_DIR}/pyspiceql/__init__.py
               COPYONLY)

# Custom Python installation target
add_custom_target(install-python
        DEPENDS pyspiceql
        COMMAND python ${CMAKE_CURRENT_BINARY_DIR}/setup.py install)