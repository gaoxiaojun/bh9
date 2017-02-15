
############ Setup project and cmake

# Project name
project (hal9000)

# Minimum cmake requirement. We should require a quite recent
# cmake for the dependency find macros etc. to be up to date.
cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

set (HAL9000_MAJOR_VERSION 0)
set (HAL9000_MINOR_VERSION 7)
set (HAL9000_PATCH_VERSION 0)
set (HAL9000_VERSION ${HAL9000_MAJOR_VERSION}.${HAL9000_MINOR_VERSION}.${HAL9000_PATCH_VERSION})

#set_property(GLOBAL PROPERTY USE_FOLDERS ON)

#set(INSTALL_INCLUDE_DIR include CACHE PATH "Installation directory for header files")
#if (WIN32 AND NOT CYGWIN)
#  set (DEF_INSTALL_CMAKE_DIR cmake)
#else ()
#  set (DEF_INSTALL_CMAKE_DIR lib/cmake/websocketpp)
#endif ()
#set (INSTALL_CMAKE_DIR ${DEF_INSTALL_CMAKE_DIR} CACHE PATH "Installation directory for CMake files")

option(Test "Build all tests." OFF)

# Set CMake library search policy
if (COMMAND cmake_policy)
    cmake_policy (SET CMP0003 NEW)
    cmake_policy (SET CMP0005 NEW)
endif ()

# Disable unnecessary build types
set (CMAKE_CONFIGURATION_TYPES "Release;RelWithDebInfo;Debug" CACHE STRING "Configurations" FORCE)

# Include our cmake macros
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
#include (CMakeHelpers)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "-g -stdlib=libc++ -Wno-unused-local-typedefs -Wno-unknown-warning-option ${CMAKE_CXX_FLAGS}")
    if ("${CMAKE_GENERATOR}" STREQUAL "Ninja")
        set(CMAKE_CXX_FLAGS "-g -fcolor-diagnostics ${CMAKE_CXX_FLAGS}")
    endif()
endif()

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    if(NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS "4.9.0") # i.e. if >= 4.9.0
        if (CMAKE_GENERATOR STREQUAL "Ninja")
            set(CMAKE_CXX_FLAGS "-g -fdiagnostics-color=always ${CMAKE_CXX_FLAGS}")
        else()
            set(CMAKE_CXX_FLAGS "-g -fdiagnostics-color=auto ${CMAKE_CXX_FLAGS}")
        endif()
    endif()
endif()

if(NOT MSVC)
    set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -Werror -Wno-unused-variable ${CMAKE_CXX_FLAGS}")
endif()

############ Depend library
## Boost
#set(BOOST_USE_STATIC_LIBS TRUE)
#set(Boost_USE_MULTITHREADED OFF)

find_package(Boost REQUIRED COMPONENTS system)
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
endif()
## Websocketpp
include_directories("3rd/websocketpp")
#find_package(Threads REQUIRED)
## OpenSSL
#find_package(OpenSSL)
#if(OpenSSL_FOUND)
#  include_directories(${OPENSSL_INCLUDE_DIR})
#endif()
## ZLIB
##find_package(ZLIB)
############ Add projects

# Add main library
add_subdirectory (3rd/backward-cpp)
add_subdirectory (3rd/websocketpp)
add_subdirectory (src/server)
add_subdirectory (broker)

################################
# Testing
################################
if(Test)
  add_subdirectory(test)
  enable_testing()
  
  # Include the gtest library. gtest_SOURCE_DIR is available due to
  # 'project(gtest)' above.
  include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

  #file(GLOB_RECURSE TEST_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} 
  #         ${CMAKE_CURRENT_SOURCE_DIR}/test/*.cpp)

  #add_executable(runUnitTests ${TEST_SRCS})

  #target_link_libraries(runUnitTests gtest gtest_main)
  
  #add_test(NAME hal9000_test COMMAND runUnitTests)
endif()

