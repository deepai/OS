cmake_minimum_required(VERSION 3.10)

# set the project name
project(Concurrency)

# add the executable
add_executable(create_destroy_threads create_destroy_threads.c)
add_executable(employee_mutex employee_mutex.c)
add_executable(thread_specific thread_specific.c)

# libraries
target_link_libraries(create_destroy_threads pthread)
target_link_libraries(employee_mutex pthread)
target_link_libraries(thread_specific pthread)