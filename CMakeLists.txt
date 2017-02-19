cmake_minimum_required(VERSION 3.1)

if ( ${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR} )
    message( FATAL_ERROR "In-source builds not allowed. Please make a new directory and run CMake from there. You may need to remove CMakeCache.txt." )
endif()

set(PROJECT_NAME h9)
project(${PROJECT_NAME})
# Location of additional cmake modules
#
set(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    ${PROJECT_SOURCE_DIR}/cmake
    )
  #~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  # This include is required if you want
  # to use CTest framework for you benchmarks with add_test()
  #
  include(CTest)

  # If you want to self-test benchmark lib too, turn me ON
  #
  set(BENCHMARK_ENABLE_TESTING OFF)

  # c++11 is required
  #
  if((CMAKE_CXX_COMPILER_ID MATCHES GNU) OR (CMAKE_CXX_COMPILER_ID MATCHES Clang))
     set(CMAKE_CXX_FLAGS         "${CMAKE_CXX_FLAGS} -std=c++11")
     set(CMAKE_CXX_FLAGS_DEBUG   "-O0 -g3")
     set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")
  endif()

  if (CMAKE_VERSION VERSION_LESS 3.2)
      set(UPDATE_DISCONNECTED_IF_AVAILABLE "")
  else()
      set(UPDATE_DISCONNECTED_IF_AVAILABLE "UPDATE_DISCONNECTED 1")
  endif()

  include(DownloadProject)

  download_project(PROJ                googletest
                   GIT_REPOSITORY      https://github.com/google/googletest.git
                   GIT_TAG             master
                   ${UPDATE_DISCONNECTED_IF_AVAILABLE}
  )

  # Prevent GoogleTest from overriding our compiler/linker options
  # when building with Visual Studio
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

  add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR})

  # When using CMake 2.8.11 or later, header path dependencies
  # are automatically added to the gtest and gmock targets.
  # For earlier CMake versions, we have to explicitly add the
  # required directories to the header search path ourselves.
  if (CMAKE_VERSION VERSION_LESS 2.8.11)
      include_directories("${gtest_SOURCE_DIR}/include"
                          "${gmock_SOURCE_DIR}/include")
  endif()

  download_project(PROJ                googlebenchmark
                   GIT_REPOSITORY      https://github.com/google/benchmark.git
                   GIT_TAG             master
                   ${UPDATE_DISCONNECTED_IF_AVAILABLE}
  )

  add_subdirectory(${googlebenchmark_SOURCE_DIR} ${googlebenchmark_BINARY_DIR})

  include_directories("${googlebenchmark_SOURCE_DIR}/include")

  add_subdirectory(${PROJECT_SOURCE_DIR}/bench/)

SET (CMAKE_LIBRARY_OUTPUT_DIRECTORY
        ${PROJECT_BINARY_DIR}/bin
        CACHE PATH
        "Single Directory for all"
    )

SET (CMAKE_RUNTIME_OUTPUT_DIRECTORY
        ${PROJECT_BINARY_DIR}/bin
        CACHE PATH
        "Single Directory for all"
    )

SET (CMAKE_ARCHIVE_OUTPUT_DIRECTORY
        ${PROJECT_BINARY_DIR}/lib
        CACHE PATH
        "Single Directory for all"
    )
set (CMAKE_CXX_STANDARD 11)
add_subdirectory (src)
add_subdirectory(test)


