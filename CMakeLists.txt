# ROOT/CMakeLists.txt

cmake_minimum_required(VERSION 2.8)

project(ASTRID)
set(CMAKE_CXX_FLAGS "-std=c++11")

set(Astrid VERSION_MAJOR 0)
set(Astrid VERSION_MINOR 1)

# TODO move this
set(CSPICE_INSTALL_DIR "/home/will/Documents/proj/traj/src/cspice")

#include the Astrid base directory so header files can be non relative
include_directories("${ASTRID_SOURCE_DIR}" "${CSPICE_INSTALL_DIR}/include")

# EDIT THIS WHEN A NEW DIRECTORY IS ADDED
# list of directories which contain astrid source code
set(ASTRID_SOURCE_DIRS "file" "util" "sim")

#initialize lists
list(APPEND ASTRID_SOURCE_FILES "")
list(APPEND ASTRID_HEADER_FILES "")

# add all cpp files in the source directories, these will be built into a library
foreach(dir ${ASTRID_SOURCE_DIRS})
  message(STATUS ${dir})
  file(GLOB_RECURSE _src_files "${dir}/*.cpp")
  #file(GLOB_RECURSE _header_files "${dir}/*.hpp")
  list(APPEND ASTRID_SOURCE_FILES ${_src_files})
  #list(APPEND ASTRID_HEADER_FILES ${_header_files})
endforeach()
message(STATUS "FOUND ALL FILES")

# create a library of all the non executeable files in the main project
add_library(libastrid STATIC ${ASTRID_SOURCE_FILES})

# add the cspice library
add_library(libcspice STATIC IMPORTED)
set_target_properties(libcspice PROPERTIES IMPORTED_LOCATION "${CSPICE_INSTALL_DIR}/lib/cspice.a")
#link_libraries(pthread libastrid "${CSPICE_INSTALL_DIR}/lib/cspice.a")

add_subdirectory(test)
