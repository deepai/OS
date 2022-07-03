cmake_minimum_required(VERSION 3.10)

# set the project name
project(memory)

add_subdirectory(Ring_buffer)
add_subdirectory(Ring_buffer_Semaphore)
add_subdirectory(Mem_Copy)
add_subdirectory(aligned_mem)
add_subdirectory(malloc_and_free)