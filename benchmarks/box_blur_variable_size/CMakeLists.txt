cmake_minimum_required(VERSION 3.13)

project(BoxBlur VERSION 1.0 LANGUAGES CXX)

add_executable(box_blur box_blur.cpp)
target_compile_features(box_blur PUBLIC cxx_std_17)
set_target_properties(box_blur PROPERTIES CXX_EXTENSIONS OFF)

target_link_libraries(box_blur PRIVATE fhecompiler)
