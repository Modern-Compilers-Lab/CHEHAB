cmake_minimum_required(VERSION 3.22.1)

project(fhecompiler_benchmarks VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(sharpening_filter)
add_subdirectory(matrix_mul)
add_subdirectory(gx_kernel)
add_subdirectory(box_blur)

add_executable(test1 test1.cpp)
add_executable(dot_product dot_product.cpp)
add_executable(reduction reduction.cpp)
add_executable(linear_regression linear_regression.cpp)


target_link_libraries(test1 fhecompiler)
target_link_libraries(dot_product fhecompiler)
target_link_libraries(reduction fhecompiler)
target_link_libraries(linear_regression fhecompiler)


