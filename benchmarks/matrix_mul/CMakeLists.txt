cmake_minimum_required(VERSION 3.22.1)

project(matrix_mul VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

add_executable(matrix_mul1 matrix_mul1.cpp)
add_executable(matrix_mul2 matrix_mul2.cpp)

target_link_libraries(matrix_mul1 fhecompiler)
target_link_libraries(matrix_mul2 fhecompiler)
