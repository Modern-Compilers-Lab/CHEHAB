cmake_minimum_required(VERSION 3.22.1)

project(sharpening_filter VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

add_executable(sharpening_filter sharpening_filter.cpp)

target_link_libraries(sharpening_filter fhecompiler)
