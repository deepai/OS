cmake_minimum_required(VERSION 3.10)

# set the project name
project(spinlock)

# add the executable
add_executable(spinlock_using_atomics main.c)

target_compile_options(spinlock_using_atomics PUBLIC -std=c11)