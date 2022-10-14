cmake_minimum_required(VERSION 3.10)

# set the project name
project(Main)

# add the nested projects
add_subdirectory(concurrency)
add_subdirectory(memory)
add_subdirectory(Timers)
add_subdirectory(atomics/spinlocks)
add_subdirectory(Geometry)
add_subdirectory(Algorithm)