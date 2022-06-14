cmake_minimum_required(VERSION 3.10)

# set the project name
project(mem_copy)

add_executable(mem_cpy main.c mem_cpy.c mem_cpy.h)
add_executable(mem_cpy_debug main.c mem_cpy.c mem_cpy.h)
target_compile_options(mem_cpy_debug PUBLIC -DDEBUG -g)