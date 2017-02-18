cmake_minimum_required(VERSION 3.1.0 FATAL_ERROR)
set (CMAKE_CXX_STANDARD 11)
enable_testing()

include_directories(../src ../src/engine)

file(GLOB_RECURSE TEST_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} 
          ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_executable(h9_test ${TEST_SRCS})
target_link_libraries(h9_test engine)
add_test(NAME h9_unit_test COMMAND h9_test)

