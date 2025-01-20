
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/benchmarks/polynomials_coyote/he)
add_executable(polynomials_coyote polynomials_coyote.cpp)
target_link_libraries(polynomials_coyote PRIVATE fheco)

# Define the source file and destination file

set(SOURCE_FILE_2 ${CMAKE_SOURCE_DIR}/benchmarks/polynomials_coyote/he/utils.cpp)
set(DESTINATION_FILE_2 ${CMAKE_SOURCE_DIR}/build/benchmarks/polynomials_coyote/he/utils.cpp)
set(SOURCE_FILE_3 ${CMAKE_SOURCE_DIR}/benchmarks/polynomials_coyote/he/utils.hpp)
set(DESTINATION_FILE_3 ${CMAKE_SOURCE_DIR}/build/benchmarks/polynomials_coyote/he/utils.hpp)

set(SOURCE_FILE_4 ${CMAKE_SOURCE_DIR}/benchmarks/polynomials_coyote/he/CMakeLists.txt)
set(DESTINATION_FILE_4 ${CMAKE_SOURCE_DIR}/build/benchmarks/polynomials_coyote/he/CMakeLists.txt)

set(SOURCE_FILE_5 ${CMAKE_SOURCE_DIR}/benchmarks/polynomials_coyote/fhe_io_example.txt)
set(DESTINATION_FILE_5 ${CMAKE_SOURCE_DIR}/build/benchmarks/polynomials_coyote/fhe_io_example.txt)


add_custom_target(copy_my_file_2polynomials_coyote ALL
    COMMAND ${CMAKE_COMMAND} -E copy ${SOURCE_FILE_2} ${DESTINATION_FILE_2}
    DEPENDS ${SOURCE_FILE}
    COMMENT "Copying ${SOURCE_FILE_2} to ${DESTINATION_FILE_2}"
)
add_custom_target(copy_my_file_3polynomials_coyote ALL
    COMMAND ${CMAKE_COMMAND} -E copy ${SOURCE_FILE_3} ${DESTINATION_FILE_3}
    DEPENDS ${SOURCE_FILE}
    COMMENT "Copying ${SOURCE_FILE_3} to ${DESTINATION_FILE_3}"
)
add_custom_target(copy_my_file_5polynomials_coyote ALL
    COMMAND ${CMAKE_COMMAND} -E copy ${SOURCE_FILE_5} ${DESTINATION_FILE_5}
    DEPENDS ${SOURCE_FILE}
    COMMENT "Copying ${SOURCE_FILE_5} to ${DESTINATION_FILE_5}"
)
set(SOURCE_FILE_4 ${CMAKE_SOURCE_DIR}/benchmarks/polynomials_coyote/he/CMakeLists.txt)
set(DESTINATION_FILE_4 ${CMAKE_SOURCE_DIR}/build/benchmarks/polynomials_coyote/he/CMakeLists.txt)
add_custom_target(copy_my_file_4polynomials_coyote ALL
    COMMAND ${CMAKE_COMMAND} -E copy ${SOURCE_FILE_4} ${DESTINATION_FILE_4}
    DEPENDS ${SOURCE_FILE}
    COMMENT "Copying ${SOURCE_FILE_4} to ${DESTINATION_FILE_4}"
)