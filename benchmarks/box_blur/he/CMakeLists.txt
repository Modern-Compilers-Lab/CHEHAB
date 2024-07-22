cmake_minimum_required(VERSION 3.13)

project(HEBoxBlur VERSION 1.0 LANGUAGES CXX)
find_package(SEAL 4.1 REQUIRED)


add_library(gen_he_box_blur STATIC _gen_he_box_blur.cpp)
target_compile_features(gen_he_box_blur PUBLIC cxx_std_17)
set_target_properties(gen_he_box_blur PROPERTIES CXX_EXTENSIONS OFF)
target_link_libraries(gen_he_box_blur SEAL::seal)


add_library(utils STATIC utils.cpp)
target_compile_features(utils PUBLIC cxx_std_17)
set_target_properties(utils PROPERTIES CXX_EXTENSIONS OFF)
target_link_libraries(utils SEAL::seal)

add_executable(main main.cpp)
target_link_libraries(main PRIVATE gen_he_box_blur)
target_link_libraries(main PRIVATE utils)

set(SOURCE_FILE_1 ${CMAKE_SOURCE_DIR}/../../../../benchmarks/box_blur/box_blur_io_example.txt)
set(DESTINATION_FILE_1 ${CMAKE_SOURCE_DIR}/box_blur_io_example.txt)
add_custom_target(copy_input_1box_blur ALL
    COMMAND ${CMAKE_COMMAND} -E copy ${SOURCE_FILE_1} ${DESTINATION_FILE_1}
    DEPENDS ${SOURCE_FILE}
    COMMENT "Copying ${SOURCE_FILE_1} to ${DESTINATION_FILE_1}"
)