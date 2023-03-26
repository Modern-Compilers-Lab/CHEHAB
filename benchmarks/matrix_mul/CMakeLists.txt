cmake_minimum_required(VERSION 3.13)

project(MatrixMul VERSION 1.0 LANGUAGES CXX)

add_executable(matrix_mul matrix_mul.cpp)
target_compile_features(matrix_mul PUBLIC cxx_std_17)
set_target_properties(matrix_mul PROPERTIES CXX_EXTENSIONS OFF)

target_link_libraries(matrix_mul PRIVATE fhecompiler)
