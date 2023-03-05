cmake_minimum_required(VERSION 3.22.1)

set(CMAKE_CXX_STANDARD 17)

add_executable(test1 test1.cpp)
add_executable(dot_product dot_product.cpp)
add_executable(matrix_mul matrix_mul.cpp)
add_executable(sharpening_filter sharpening_filter.cpp)
add_executable(box_blur box_blur.cpp)
add_executable(gx_kernel gx_kernel.cpp)
add_executable(reduction reduction.cpp)
add_executable(linear_regression linear_regression.cpp)
add_executable(matrix_mul_plain matrix_mul_plain.cpp)


target_link_libraries(test1 fhecompiler)
target_link_libraries(dot_product fhecompiler)
target_link_libraries(matrix_mul fhecompiler)
target_link_libraries(sharpening_filter fhecompiler)
target_link_libraries(box_blur fhecompiler)
target_link_libraries(gx_kernel fhecompiler)
target_link_libraries(reduction fhecompiler)
target_link_libraries(linear_regression fhecompiler)
target_link_libraries(matrix_mul_plain fhecompiler)

