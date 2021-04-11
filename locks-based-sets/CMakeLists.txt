cmake_minimum_required(VERSION 3.10)

project(LocksBasedSets)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(include)

add_executable(LocksBasedSets main.cpp include/nodes.hpp include/sets.hpp)
