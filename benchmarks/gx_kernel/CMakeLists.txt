
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/benchmarks/gx_kernel/he)
add_executable(gx_kernel gx_kernel.cpp)
target_link_libraries(gx_kernel PRIVATE fheco)

# Define the source file and destination file 



set(SOURCE_FILE_2 ${CMAKE_SOURCE_DIR}/benchmarks/gx_kernel/he/utils.cpp)
set(DESTINATION_FILE_2 ${CMAKE_SOURCE_DIR}/build/benchmarks/gx_kernel/he/utils.cpp)

set(SOURCE_FILE_3 ${CMAKE_SOURCE_DIR}/benchmarks/gx_kernel/he/utils.hpp)
set(DESTINATION_FILE_3 ${CMAKE_SOURCE_DIR}/build/benchmarks/gx_kernel/he/utils.hpp)

set(SOURCE_FILE_4 ${CMAKE_SOURCE_DIR}/benchmarks/gx_kernel/he/CMakeLists.txt)
set(DESTINATION_FILE_4 ${CMAKE_SOURCE_DIR}/build/benchmarks/gx_kernel/he/CMakeLists.txt)

set(SOURCE_FILE_5 ${CMAKE_SOURCE_DIR}/benchmarks/gx_kernel/fhe_io_example.txt)
set(DESTINATION_FILE_5 ${CMAKE_SOURCE_DIR}/build/benchmarks/gx_kernel/fhe_io_example.txt)

set(SOURCE_FILE_6 ${CMAKE_SOURCE_DIR}/benchmarks/gx_kernel/generate_gx_kernel.py)
set(DESTINATION_FILE_6 ${CMAKE_SOURCE_DIR}/build/benchmarks/gx_kernel/generate_gx_kernel.py)

add_custom_target(copy_my_file_6gx_kernel ALL 
    COMMAND ${CMAKE_COMMAND} -E copy ${SOURCE_FILE_6} ${DESTINATION_FILE_6}
    DEPENDS ${SOURCE_FILE}
    COMMENT "Copying ${SOURCE_FILE_6} to ${DESTINATION_FILE_6}"
)

add_custom_target(copy_my_file_2gx_kernel ALL
    COMMAND ${CMAKE_COMMAND} -E copy ${SOURCE_FILE_2} ${DESTINATION_FILE_2}
    DEPENDS ${SOURCE_FILE}
    COMMENT "Copying ${SOURCE_FILE_2} to ${DESTINATION_FILE_2}"
)
add_custom_target(copy_my_file_3gx_kernel ALL
    COMMAND ${CMAKE_COMMAND} -E copy ${SOURCE_FILE_3} ${DESTINATION_FILE_3}
    DEPENDS ${SOURCE_FILE}
    COMMENT "Copying ${SOURCE_FILE_3} to ${DESTINATION_FILE_3}"
)

add_custom_target(copy_my_file_5gx_kernel ALL 
    COMMAND ${CMAKE_COMMAND} -E copy ${SOURCE_FILE_5} ${DESTINATION_FILE_5}
    DEPENDS ${SOURCE_FILE}
    COMMENT "Copying ${SOURCE_FILE_5} to ${DESTINATION_FILE_5}"
)

set(SOURCE_FILE_4 ${CMAKE_SOURCE_DIR}/benchmarks/gx_kernel/he/CMakeLists.txt)
set(DESTINATION_FILE_4 ${CMAKE_SOURCE_DIR}/build/benchmarks/gx_kernel/he/CMakeLists.txt)
add_custom_target(copy_my_file_4gx_kernel ALL
    COMMAND ${CMAKE_COMMAND} -E copy ${SOURCE_FILE_4} ${DESTINATION_FILE_4}
    DEPENDS ${SOURCE_FILE}
    COMMENT "Copying ${SOURCE_FILE_4} to ${DESTINATION_FILE_4}"
)