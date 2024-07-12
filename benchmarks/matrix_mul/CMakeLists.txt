add_executable(matrix_mul matrix_mul.cpp)
target_link_libraries(matrix_mul PRIVATE fhecompiler)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/benchmarks/matrix_mul/he)


set(SOURCE_FILE_1 ${CMAKE_SOURCE_DIR}/benchmarks/matrix_mul/he/main.cpp)
set(DESTINATION_FILE_1 ${CMAKE_SOURCE_DIR}/build/benchmarks/matrix_mul/he/main.cpp)
set(SOURCE_FILE_2 ${CMAKE_SOURCE_DIR}/benchmarks/matrix_mul/he/utils.cpp)
set(DESTINATION_FILE_2 ${CMAKE_SOURCE_DIR}/build/benchmarks/matrix_mul/he/utils.cpp)
set(SOURCE_FILE_3 ${CMAKE_SOURCE_DIR}/benchmarks/matrix_mul/he/utils.hpp)
set(DESTINATION_FILE_3 ${CMAKE_SOURCE_DIR}/build/benchmarks/matrix_mul/he/utils.hpp)

add_custom_target(copy_my_file_1matrix_mul ALL
    COMMAND ${CMAKE_COMMAND} -E copy ${SOURCE_FILE_1} ${DESTINATION_FILE_1}
    DEPENDS ${SOURCE_FILE}
    COMMENT "Copying ${SOURCE_FILE_1} to ${DESTINATION_FILE_1}"
)
add_custom_target(copy_my_file_2matrix_mul ALL
    COMMAND ${CMAKE_COMMAND} -E copy ${SOURCE_FILE_2} ${DESTINATION_FILE_2}
    DEPENDS ${SOURCE_FILE}
    COMMENT "Copying ${SOURCE_FILE_2} to ${DESTINATION_FILE_2}"
)
add_custom_target(copy_my_file_3matrix_mul ALL
    COMMAND ${CMAKE_COMMAND} -E copy ${SOURCE_FILE_3} ${DESTINATION_FILE_3}
    DEPENDS ${SOURCE_FILE}
    COMMENT "Copying ${SOURCE_FILE_3} to ${DESTINATION_FILE_3}"
)

set(SOURCE_FILE_4 ${CMAKE_SOURCE_DIR}/benchmarks/matrix_mul/he/CMakeLists.txt)
set(DESTINATION_FILE_4 ${CMAKE_SOURCE_DIR}/build/benchmarks/matrix_mul/he/CMakeLists.txt)
add_custom_target(copy_my_file_4matrix_mul ALL
    COMMAND ${CMAKE_COMMAND} -E copy ${SOURCE_FILE_4} ${DESTINATION_FILE_4}
    DEPENDS ${SOURCE_FILE}
    COMMENT "Copying ${SOURCE_FILE_4} to ${DESTINATION_FILE_4}"
)