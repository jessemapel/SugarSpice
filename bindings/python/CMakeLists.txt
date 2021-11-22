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
                      io.i
                      query.i
                      spice_types.i
                      utils.i)
set_source_files_properties(${PYSPICEQL_SOURCES} PROPERTIES CPLUSPLUS ON)
swig_add_library(pyspiceql
                 LANGUAGE python
                 SOURCES ${PYSPICEQL_SOURCES})
swig_link_libraries(pyspiceql SpiceQL Python3::Module)
set_property(TARGET pyspiceql PROPERTY SWIG_USE_TARGET_INCLUDE_DIRECTORIES TRUE)

# Setup for wrapper tests
if(SPICEQL_BUILD_TESTS)
    add_test(NAME python-tests
        COMMAND python -m pytest ${CMAKE_CURRENT_SOURCE_DIR}/tests
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
else()
    message(STATUS "Skipping Python Bindings Tests")
endif()

# Configure Python install files
set(PYTHON_INSTALL_FILES
        ${CMAKE_CURRENT_BINARY_DIR}/pyspiceql.py
        ${CMAKE_CURRENT_BINARY_DIR}/_pyspiceql.so)
set(SETUP_PY_IN ${CMAKE_CURRENT_SOURCE_DIR}/setup.py.in)
set(SETUP_PY_OUT ${CMAKE_CURRENT_BINARY_DIR}/setup.py)
configure_file(${SETUP_PY_IN} ${SETUP_PY_OUT})
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/__init__.py.in ${CMAKE_CURRENT_BINARY_DIR}/__init__.py)
set_target_properties(_pyspiceql PROPERTIES INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")

# Custom Python installation target
add_custom_target(install-python
        DEPENDS pyspiceql
        COMMAND python ${SETUP_PY_OUT} install)