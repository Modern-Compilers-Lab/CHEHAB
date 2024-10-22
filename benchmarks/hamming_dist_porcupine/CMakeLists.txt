add_executable(hamming_dist_porcupine_checks hamming_dist_checks.cpp)
target_link_libraries(hamming_dist_porcupine_checks PRIVATE fheco)

add_executable(hamming_dist_porcupine hamming_dist.cpp)
target_link_libraries(hamming_dist_porcupine PRIVATE fheco)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/benchmarks/hamming_dist_porcupine/he)

set(SOURCE_FILE_1 ${CMAKE_SOURCE_DIR}/benchmarks/hamming_dist_porcupine/he/main.cpp)
set(DESTINATION_FILE_1 ${CMAKE_SOURCE_DIR}/build/benchmarks/hamming_dist_porcupine/he/main.cpp)
set(SOURCE_FILE_2 ${CMAKE_SOURCE_DIR}/benchmarks/hamming_dist_porcupine/he/utils.cpp)
set(DESTINATION_FILE_2 ${CMAKE_SOURCE_DIR}/benchmarks/hamming_dist_porcupine/he/utils.cpp)
set(SOURCE_FILE_3 ${CMAKE_SOURCE_DIR}/benchmarks/hamming_dist_porcupine/he/utils.hpp)
set(DESTINATION_FILE_3 ${CMAKE_SOURCE_DIR}/build/benchmarks/hamming_dist_porcupine/he/utils.hpp)

add_custom_target(copy_my_file_1hamming_dist_porcupine ALL
    COMMAND ${CMAKE_COMMAND} -E copy ${SOURCE_FILE_1} ${DESTINATION_FILE_1}
    DEPENDS ${SOURCE_FILE}
    COMMENT "Copying ${SOURCE_FILE_1} to ${DESTINATION_FILE_1}"
)
add_custom_target(copy_my_file_2hamming_dist_porcupine ALL
    COMMAND ${CMAKE_COMMAND} -E copy ${SOURCE_FILE_2} ${DESTINATION_FILE_2}
    DEPENDS ${SOURCE_FILE}
    COMMENT "Copying ${SOURCE_FILE_2} to ${DESTINATION_FILE_2}"
)
add_custom_target(copy_my_file_3hamming_dist_porcupine ALL
    COMMAND ${CMAKE_COMMAND} -E copy ${SOURCE_FILE_3} ${DESTINATION_FILE_3}
    DEPENDS ${SOURCE_FILE}
    COMMENT "Copying ${SOURCE_FILE_3} to ${DESTINATION_FILE_3}"
)


set(SOURCE_FILE_4 ${CMAKE_SOURCE_DIR}/benchmarks/hamming_dist_porcupine/he/CMakeLists.txt)
set(DESTINATION_FILE_4 ${CMAKE_SOURCE_DIR}/build/benchmarks/hamming_dist_porcupine/he/CMakeLists.txt)
add_custom_target(copy_my_file_4hamming_dist_porcupine ALL
    COMMAND ${CMAKE_COMMAND} -E copy ${SOURCE_FILE_4} ${DESTINATION_FILE_4}
    DEPENDS ${SOURCE_FILE}
    COMMENT "Copying ${SOURCE_FILE_4} to ${DESTINATION_FILE_4}"
)