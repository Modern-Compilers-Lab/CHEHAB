add_executable(sharpening_filter sharpening_filter.cpp)
target_link_libraries(sharpening_filter PRIVATE fhecompiler)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/benchmarks/sharpening_filter/he)



set(SOURCE_FILE_1 ${CMAKE_SOURCE_DIR}/benchmarks/sharpening_filter/he/main.cpp)
set(DESTINATION_FILE_1 ${CMAKE_SOURCE_DIR}/build/benchmarks/sharpening_filter/he/main.cpp)
set(SOURCE_FILE_2 ${CMAKE_SOURCE_DIR}/benchmarks/sharpening_filter/he/utils.cpp)
set(DESTINATION_FILE_2 ${CMAKE_SOURCE_DIR}/build/benchmarks/sharpening_filter/he/utils.cpp)
set(SOURCE_FILE_3 ${CMAKE_SOURCE_DIR}/benchmarks/sharpening_filter/he/utils.hpp)
set(DESTINATION_FILE_3 ${CMAKE_SOURCE_DIR}/build/benchmarks/sharpening_filter/he/utils.hpp)

add_custom_target(copy_my_file_1sharpening_filter ALL
    COMMAND ${CMAKE_COMMAND} -E copy ${SOURCE_FILE_1} ${DESTINATION_FILE_1}
    DEPENDS ${SOURCE_FILE}
    COMMENT "Copying ${SOURCE_FILE_1} to ${DESTINATION_FILE_1}"
)
add_custom_target(copy_my_file_2sharpening_filter ALL
    COMMAND ${CMAKE_COMMAND} -E copy ${SOURCE_FILE_2} ${DESTINATION_FILE_2}
    DEPENDS ${SOURCE_FILE}
    COMMENT "Copying ${SOURCE_FILE_2} to ${DESTINATION_FILE_2}"
)
add_custom_target(copy_my_file_3sharpening_filter ALL
    COMMAND ${CMAKE_COMMAND} -E copy ${SOURCE_FILE_3} ${DESTINATION_FILE_3}
    DEPENDS ${SOURCE_FILE}
    COMMENT "Copying ${SOURCE_FILE_3} to ${DESTINATION_FILE_3}"
)

set(SOURCE_FILE_4 ${CMAKE_SOURCE_DIR}/benchmarks/sharpening_filter_heco/he/CMakeLists.txt)
set(DESTINATION_FILE_4 ${CMAKE_SOURCE_DIR}/build/benchmarks/sharpening_filter_heco/he/CMakeLists.txt)
add_custom_target(copy_my_file_4sharpening_filter_heco ALL
    COMMAND ${CMAKE_COMMAND} -E copy ${SOURCE_FILE_4} ${DESTINATION_FILE_4}
    DEPENDS ${SOURCE_FILE}
    COMMENT "Copying ${SOURCE_FILE_4} to ${DESTINATION_FILE_4}"
)