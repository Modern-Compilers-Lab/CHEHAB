add_executable(dot_product_porcupine dot_product.cpp)
target_link_libraries(dot_product_porcupine PRIVATE fheco)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/benchmarks/dot_product_porcupine/he)

set(SOURCE_FILE_1 ${CMAKE_SOURCE_DIR}/benchmarks/dot_product_porcupine/he/main.cpp)
set(DESTINATION_FILE_1 ${CMAKE_SOURCE_DIR}/build/benchmarks/dot_product_porcupine/he/main.cpp)
set(SOURCE_FILE_2 ${CMAKE_SOURCE_DIR}/benchmarks/dot_product_porcupine/he/utils.cpp)
set(DESTINATION_FILE_2 ${CMAKE_SOURCE_DIR}/build/benchmarks/dot_product_porcupine/he/utils.cpp)
set(SOURCE_FILE_3 ${CMAKE_SOURCE_DIR}/benchmarks/dot_product_porcupine/he/utils.hpp)
set(DESTINATION_FILE_3 ${CMAKE_SOURCE_DIR}/build/benchmarks/dot_product_porcupine/he/utils.hpp)

add_custom_target(copy_my_file_1dot_product_porcupine ALL
    COMMAND ${CMAKE_COMMAND} -E copy ${SOURCE_FILE_1} ${DESTINATION_FILE_1}
    DEPENDS ${SOURCE_FILE}
    COMMENT "Copying ${SOURCE_FILE_1} to ${DESTINATION_FILE_1}"
)
add_custom_target(copy_my_file_2dot_product_porcupine ALL
    COMMAND ${CMAKE_COMMAND} -E copy ${SOURCE_FILE_2} ${DESTINATION_FILE_2}
    DEPENDS ${SOURCE_FILE}
    COMMENT "Copying ${SOURCE_FILE_2} to ${DESTINATION_FILE_2}"
)
add_custom_target(copy_my_file_3dot_product_porcupine ALL
    COMMAND ${CMAKE_COMMAND} -E copy ${SOURCE_FILE_3} ${DESTINATION_FILE_3}
    DEPENDS ${SOURCE_FILE}
    COMMENT "Copying ${SOURCE_FILE_3} to ${DESTINATION_FILE_3}"
)

set(SOURCE_FILE_4 ${CMAKE_SOURCE_DIR}/benchmarks/dot_product_porcupine/he/CMakeLists.txt)
set(DESTINATION_FILE_4 ${CMAKE_SOURCE_DIR}/build/benchmarks/dot_product_porcupine/he/CMakeLists.txt)
add_custom_target(copy_my_file_4dot_product_porcupine ALL
    COMMAND ${CMAKE_COMMAND} -E copy ${SOURCE_FILE_4} ${DESTINATION_FILE_4}
    DEPENDS ${SOURCE_FILE}
    COMMENT "Copying ${SOURCE_FILE_4} to ${DESTINATION_FILE_4}"
)