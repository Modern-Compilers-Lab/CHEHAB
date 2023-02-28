cmake_minimum_required(VERSION 3.22.1)

project(gx_kernel VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

add_executable(gx_kernel gx_kernel.cpp)

target_link_libraries(gx_kernel fhecompiler)
