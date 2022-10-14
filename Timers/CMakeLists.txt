cmake_minimum_required(VERSION 3.10)

# set the project name
project(timers)

set(CMAKE_BUILD_TYPE RelWithDebInfo)

# add the executable
add_executable(timer simple_timer.cpp timerlib.c timerlib.h)
add_executable(timer_hierarchy timer_hierarchy.cpp timerlib.c timerlib.h)

# libraries
target_link_libraries(timer pthread rt)
target_link_libraries(timer_hierarchy pthread rt)