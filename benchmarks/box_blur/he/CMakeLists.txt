cmake_minimum_required(VERSION 3.13)

project(HEBoxBlur VERSION 1.0 LANGUAGES CXX)

SET(SEAL_HEADERS "" CACHE STRING "SEAl headers path")
SET(SEAL_OBJECT "" CACHE STRING "SEAl object path")

add_library(gen_he_box_blur_noopt STATIC gen_he_box_blur_noopt.cpp)
target_compile_features(gen_he_box_blur_noopt PUBLIC cxx_std_17)
set_target_properties(gen_he_box_blur_noopt PROPERTIES CXX_EXTENSIONS OFF)
target_include_directories(gen_he_box_blur_noopt PUBLIC ${SEAL_HEADERS})
target_link_libraries(gen_he_box_blur_noopt PUBLIC ${SEAL_OBJECT})

add_library(gen_he_box_blur_opt STATIC gen_he_box_blur_opt.cpp)
target_compile_features(gen_he_box_blur_opt PUBLIC cxx_std_17)
set_target_properties(gen_he_box_blur_opt PROPERTIES CXX_EXTENSIONS OFF)
target_include_directories(gen_he_box_blur_opt PUBLIC ${SEAL_HEADERS})
target_link_libraries(gen_he_box_blur_opt PUBLIC ${SEAL_OBJECT})

add_library(utils STATIC utils.cpp)
target_compile_features(utils PUBLIC cxx_std_17)
set_target_properties(utils PROPERTIES CXX_EXTENSIONS OFF)
target_include_directories(utils PUBLIC ${SEAL_HEADERS})
target_link_libraries(utils PUBLIC ${SEAL_OBJECT})

add_executable(main main.cpp)
target_link_libraries(main PRIVATE gen_he_box_blur_noopt)
target_link_libraries(main PRIVATE gen_he_box_blur_opt)
target_link_libraries(main PRIVATE utils)
