add_executable(matrix_mul_poc matrix_mul.cpp)
add_executable(matrix_mul_poc_log2 matrix_mul_poc_log2.cpp)

target_link_libraries(matrix_mul_poc fhecompiler)
target_link_libraries(matrix_mul_poc_log2 fhecompiler)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/benchmarks/matrix_mul_poc/he)

set(SOURCE_FILE_1 ${CMAKE_SOURCE_DIR}/benchmarks/matrix_mul_poc/he/main.cpp)
set(DESTINATION_FILE_1 ${CMAKE_SOURCE_DIR}/build/benchmarks/matrix_mul_poc/he/main.cpp)
set(SOURCE_FILE_2 ${CMAKE_SOURCE_DIR}/benchmarks/matrix_mul_poc/he/utils.cpp)
set(DESTINATION_FILE_2 ${CMAKE_SOURCE_DIR}/build/benchmarks/matrix_mul_poc/he/utils.cpp)
set(SOURCE_FILE_3 ${CMAKE_SOURCE_DIR}/benchmarks/matrix_mul_poc/he/utils.hpp)
set(DESTINATION_FILE_3 ${CMAKE_SOURCE_DIR}/build/benchmarks/matrix_mul_poc/he/utils.hpp)

add_custom_target(copy_my_file_1matrix_mul_poc ALL
    COMMAND ${CMAKE_COMMAND} -E copy ${SOURCE_FILE_1} ${DESTINATION_FILE_1}
    DEPENDS ${SOURCE_FILE}
    COMMENT "Copying ${SOURCE_FILE_1} to ${DESTINATION_FILE_1}"
)
add_custom_target(copy_my_file_2matrix_mul_poc ALL
    COMMAND ${CMAKE_COMMAND} -E copy ${SOURCE_FILE_2} ${DESTINATION_FILE_2}
    DEPENDS ${SOURCE_FILE}
    COMMENT "Copying ${SOURCE_FILE_2} to ${DESTINATION_FILE_2}"
)
add_custom_target(copy_my_file_3matrix_mul_poc ALL
    COMMAND ${CMAKE_COMMAND} -E copy ${SOURCE_FILE_3} ${DESTINATION_FILE_3}
    DEPENDS ${SOURCE_FILE}
    COMMENT "Copying ${SOURCE_FILE_3} to ${DESTINATION_FILE_3}"
)