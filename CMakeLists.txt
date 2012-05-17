include (${CMAKE_CURRENT_SOURCE_DIR}/modules/FindBoost.cmake)

find_package (Boost 1.45 REQUIRED)
include_directories (${Boost_INCLUDE_DIR})

set (sources "binary_pattern_replace.cpp")

add_executable (binary_pattern_replace ${sources})
