cmake_minimum_required (VERSION 3.10)
project(waves)

include(${CMAKE_SOURCE_DIR}/cmake/globalVariables.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/install.cmake)

add_subdirectory(src)
