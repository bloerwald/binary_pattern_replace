cmake_minimum_required (VERSION 2.8)

include (${CMAKE_CURRENT_SOURCE_DIR}/modules/FindBoost.cmake)

find_package (Boost 1.45 REQUIRED)
include_directories (${Boost_INCLUDE_DIR})

if (NOT WIN32) # for gcc only, msvc++ seems to default to c++11.
  set (CMAKE_CXX_FLAGS "-std=c++11" ${CMAKE_CXX_FLAGS})
endif()

add_executable (binary_pattern_replace "binary_pattern_replace.cpp")
