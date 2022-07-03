cmake_minimum_required(VERSION 3.10)

# set the project name
project(malloc_and_free)

add_executable(malloc_and_free main.c custom_malloc.c custom_malloc.h)
add_executable(malloc_and_free_debug main.c custom_malloc.c custom_malloc.h)

target_compile_options(malloc_and_free_debug PUBLIC -DDEBUG -g)