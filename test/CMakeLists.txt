# ROOT/tests/CMakeLists.txt

##load the Google test package
##find_package(GTest REQUIRED)
#locate Google moc libs
set (GMOCK_ROOT "/usr/src/gmock") 
message(STATUS "Using google mock root dir: ${GMOCK_ROOT}")

#link_directories(${GMOCK_DIR})

include_directories("${GMOCK_ROOT}/gtest/include/")

link_libraries("${GMOCK_ROOT}/build/libgmock.a" "${GMOCK_ROOT}/build/gtest/libgtest.a" pthread libastrid libcspice)

message(STATUS ${CMAKE_CURRENT_SOURCE_DIR})

# find all executeable files
file(GLOB_RECURSE ASTRID_TEST_SOURCES "*.cxx")
message(STATUS "Test source: ${ASTRID_TEST_SOURCES}")  

enable_testing()

# create an executable and a corresponding test for each .cxx file
foreach(_test ${ASTRID_TEST_SOURCES})
  get_filename_component(TEST_NAME ${_test} NAME_WE) # set the name of the test to the name of the file
  message(STATUS "Directory of test: ${TEST_DIR}")
  message(STATUS "Name of test: ${TEST_NAME}")
  add_executable("${TEST_NAME}" ${_test})
  add_test(${TEST_NAME} "${TEST_NAME}")
endforeach()

