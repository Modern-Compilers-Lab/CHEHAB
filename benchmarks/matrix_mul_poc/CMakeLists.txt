set(CMAKE_CXX_STANDARD 17)

include_directories(/usr/local/include/FHECO)

add_executable(matrix_mul matrix_mul.cpp)
target_link_libraries(matrix_mul PRIVATE fheco)