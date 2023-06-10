cmake_minimum_required(VERSION 3.13)

project(HEBoxBlur VERSION 1.0 LANGUAGES CXX)

find_package(SEAL 4.1.1 EXACT REQUIRED)

add_library(gen_he_box_blur_baseline STATIC gen_he_box_blur_baseline.cpp)
target_compile_features(gen_he_box_blur_baseline PUBLIC cxx_std_17)
set_target_properties(gen_he_box_blur_baseline PROPERTIES CXX_EXTENSIONS OFF)
target_link_libraries(gen_he_box_blur_baseline PUBLIC SEAL::seal)

add_library(gen_he_box_blur STATIC gen_he_box_blur.cpp)
target_compile_features(gen_he_box_blur PUBLIC cxx_std_17)
set_target_properties(gen_he_box_blur PROPERTIES CXX_EXTENSIONS OFF)
target_link_libraries(gen_he_box_blur PUBLIC SEAL::seal)

add_library(utils STATIC utils.cpp)
target_compile_features(utils PUBLIC cxx_std_17)
set_target_properties(utils PROPERTIES CXX_EXTENSIONS OFF)
target_link_libraries(utils PUBLIC SEAL::seal)

add_executable(main_baseline main_baseline.cpp)
target_link_libraries(main_baseline PRIVATE gen_he_box_blur_baseline)
target_link_libraries(main_baseline PRIVATE utils)

add_executable(main_opt main_opt.cpp)
target_link_libraries(main_opt PRIVATE gen_he_box_blur)
target_link_libraries(main_opt PRIVATE utils)

