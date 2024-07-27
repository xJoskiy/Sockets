cmake_minimum_required(VERSION 3.22)

set(CMAKE_CXX_STANDARD_REQUIRED 14)

project(socket_interaction)

add_subdirectory(prog1)
add_subdirectory(prog2)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY}")

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif(NOT CMAKE_BUILD_TYPE)
