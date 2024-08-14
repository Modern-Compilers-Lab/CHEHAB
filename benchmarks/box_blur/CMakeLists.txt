
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/benchmarks/box_blur/he)
add_executable(box_blur box_blur.cpp)
target_link_libraries(box_blur PRIVATE fheco)

# Define the source file and destination file

set(SOURCE_FILE_1 ${CMAKE_SOURCE_DIR}/benchmarks/box_blur/he/main.cpp)
set(DESTINATION_FILE_1 ${CMAKE_SOURCE_DIR}/build/benchmarks/box_blur/he/main.cpp)
set(SOURCE_FILE_2 ${CMAKE_SOURCE_DIR}/benchmarks/box_blur/he/utils.cpp)
set(DESTINATION_FILE_2 ${CMAKE_SOURCE_DIR}/build/benchmarks/box_blur/he/utils.cpp)
set(SOURCE_FILE_3 ${CMAKE_SOURCE_DIR}/benchmarks/box_blur/he/utils.hpp)
set(DESTINATION_FILE_3 ${CMAKE_SOURCE_DIR}/build/benchmarks/box_blur/he/utils.hpp)
set(SOURCE_FILE_4 ${CMAKE_SOURCE_DIR}/benchmarks/box_blur/he/CMakeLists.txt)
set(DESTINATION_FILE_4 ${CMAKE_SOURCE_DIR}/build/benchmarks/box_blur/he/CMakeLists.txt)

add_custom_target(copy_my_file_1box_blur ALL
    COMMAND ${CMAKE_COMMAND} -E copy ${SOURCE_FILE_1} ${DESTINATION_FILE_1}
    DEPENDS ${SOURCE_FILE}
    COMMENT "Copying ${SOURCE_FILE_1} to ${DESTINATION_FILE_1}"
)
add_custom_target(copy_my_file_2box_blur ALL
    COMMAND ${CMAKE_COMMAND} -E copy ${SOURCE_FILE_2} ${DESTINATION_FILE_2}
    DEPENDS ${SOURCE_FILE}
    COMMENT "Copying ${SOURCE_FILE_2} to ${DESTINATION_FILE_2}"
)
add_custom_target(copy_my_file_3box_blur ALL
    COMMAND ${CMAKE_COMMAND} -E copy ${SOURCE_FILE_3} ${DESTINATION_FILE_3}
    DEPENDS ${SOURCE_FILE}
    COMMENT "Copying ${SOURCE_FILE_3} to ${DESTINATION_FILE_3}"
)

set(SOURCE_FILE_4 ${CMAKE_SOURCE_DIR}/benchmarks/box_blur/he/CMakeLists.txt)
set(DESTINATION_FILE_4 ${CMAKE_SOURCE_DIR}/build/benchmarks/box_blur/he/CMakeLists.txt)
add_custom_target(copy_my_file_4box_blur ALL
    COMMAND ${CMAKE_COMMAND} -E copy ${SOURCE_FILE_4} ${DESTINATION_FILE_4}
    DEPENDS ${SOURCE_FILE}
    COMMENT "Copying ${SOURCE_FILE_4} to ${DESTINATION_FILE_4}"
)