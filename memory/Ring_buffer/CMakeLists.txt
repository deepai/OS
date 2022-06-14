cmake_minimum_required(VERSION 3.10)

# set the project name
project(ring_buffer)

# add the executable
add_executable(ring_buffer main.c ring_buffer.c ring_buffer.h)
add_executable(ring_buffer_debug main.c ring_buffer.c ring_buffer.h)

# libraries
target_link_libraries(ring_buffer pthread)
target_link_libraries(ring_buffer_debug pthread)
target_compile_options(ring_buffer_debug PUBLIC -DDEBUG -g)