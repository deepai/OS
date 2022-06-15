cmake_minimum_required(VERSION 3.10)

# set the project name
project(ring_buffer)

# add the executable
add_executable(ring_buffer_sem main.c ring_buffer.c ring_buffer.h)
add_executable(ring_buffer_debug_sem main.c ring_buffer.c ring_buffer.h)

# libraries
target_link_libraries(ring_buffer_sem pthread)
target_link_libraries(ring_buffer_debug_sem pthread)
target_compile_options(ring_buffer_debug_sem PUBLIC -DDEBUG -g)