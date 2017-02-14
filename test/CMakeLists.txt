cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

enable_testing()

file(GLOB_RECURSE TEST_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} 
          ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_executable(h9_test ${TEST_SRCS})
  
add_test(NAME h9_unit_test COMMAND h9_test)

