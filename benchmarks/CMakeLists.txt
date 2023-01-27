cmake_minimum_required(VERSION 3.22.1)

set(CMAKE_CXX_STANDARD 17)

add_executable(test1 test1.cpp)
add_executable(dot_product dot_product.cpp)
add_executable(matrix_mul matrix_mul.cpp)

target_link_libraries(test1 fhecompiler)
target_link_libraries(dot_product fhecompiler)
target_link_libraries(matrix_mul fhecompiler)
