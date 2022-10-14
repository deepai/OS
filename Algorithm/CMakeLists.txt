cmake_minimum_required(VERSION 3.10)

# set the project name
project(Algorithms)

add_subdirectory(Bitwise)

add_executable(kmp kmp.cpp)
add_executable(rabinkarp rabinkarp.cpp)
add_executable(closesPairOfPoints closesPairOfPoints.cpp)