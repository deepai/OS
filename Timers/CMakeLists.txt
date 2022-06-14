cmake_minimum_required(VERSION 3.10)

# set the project name
project(timers)

# add the executable
add_executable(timer simple_timer.c timerlib.c timerlib.h)

# libraries
target_link_libraries(timer pthread rt)