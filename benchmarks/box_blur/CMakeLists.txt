cmake_minimum_required(VERSION 3.22.1)

project(box_blur VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

add_executable(box_blur box_blur.cpp)

target_link_libraries(box_blur fhecompiler)
