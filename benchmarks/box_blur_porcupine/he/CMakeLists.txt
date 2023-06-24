cmake_minimum_required(VERSION 3.13)

project(HEBoxBlur VERSION 1.0 LANGUAGES CXX)

find_package(SEAL 4.1.1 EXACT REQUIRED)

add_library(gen_he_box_blur_noopt STATIC gen_he_box_blur_noopt.cpp)
target_compile_features(gen_he_box_blur_noopt PUBLIC cxx_std_17)
set_target_properties(gen_he_box_blur_noopt PROPERTIES CXX_EXTENSIONS OFF)
target_link_libraries(gen_he_box_blur_noopt PUBLIC SEAL::seal)

add_library(gen_he_box_blur STATIC gen_he_box_blur.cpp)
target_compile_features(gen_he_box_blur PUBLIC cxx_std_17)
set_target_properties(gen_he_box_blur PROPERTIES CXX_EXTENSIONS OFF)
target_link_libraries(gen_he_box_blur PUBLIC SEAL::seal)

add_library(utils STATIC utils.cpp)
target_compile_features(utils PUBLIC cxx_std_17)
set_target_properties(utils PROPERTIES CXX_EXTENSIONS OFF)
target_link_libraries(utils PUBLIC SEAL::seal)

add_executable(main_noopt main_noopt.cpp)
target_link_libraries(main_noopt PRIVATE gen_he_box_blur_noopt)
target_link_libraries(main_noopt PRIVATE utils)

add_executable(main_opt main_opt.cpp)
target_link_libraries(main_opt PRIVATE gen_he_box_blur)
target_link_libraries(main_opt PRIVATE utils)
