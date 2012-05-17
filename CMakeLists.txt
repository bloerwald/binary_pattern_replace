include (${CMAKE_CURRENT_SOURCE_DIR}/modules/FindBoost.cmake)

find_package (Boost 1.45 REQUIRED)
include_directories (${Boost_INCLUDE_DIR})

set (CMAKE_CXX_FLAGS "-std=c++11" ${CMAKE_CXX_FLAGS})

add_executable (binary_pattern_replace "binary_pattern_replace.cpp")
