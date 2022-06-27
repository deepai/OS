cmake_minimum_required(VERSION 3.10)

# set the project name
project(producer_consumer)

set(CMAKE_BUILD_TYPE RelWithDebInfo)

# add the executable
add_executable(single_producer_consumer single_producer_consumer.c ring_buffer.c ring_buffer.h)
add_executable(multiple_produer_consumer multiple_producer_single_consumer.c ring_buffer.c ring_buffer.h)


# libraries
target_link_libraries(single_producer_consumer pthread)
target_link_libraries(multiple_produer_consumer pthread)