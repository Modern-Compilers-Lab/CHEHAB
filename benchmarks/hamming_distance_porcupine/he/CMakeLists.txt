cmake_minimum_required(VERSION 3.13)

project(HEHammingDistance VERSION 1.0 LANGUAGES CXX)

find_package(SEAL 4.1.1 EXACT REQUIRED)

add_library(gen_he_hamming_distance_noopt STATIC gen_he_hamming_distance_noopt.cpp)
target_compile_features(gen_he_hamming_distance_noopt PUBLIC cxx_std_17)
set_target_properties(gen_he_hamming_distance_noopt PROPERTIES CXX_EXTENSIONS OFF)
target_link_libraries(gen_he_hamming_distance_noopt PUBLIC SEAL::seal)

add_library(gen_he_hamming_distance STATIC gen_he_hamming_distance.cpp)
target_compile_features(gen_he_hamming_distance PUBLIC cxx_std_17)
set_target_properties(gen_he_hamming_distance PROPERTIES CXX_EXTENSIONS OFF)
target_link_libraries(gen_he_hamming_distance PUBLIC SEAL::seal)

add_library(utils STATIC utils.cpp)
target_compile_features(utils PUBLIC cxx_std_17)
set_target_properties(utils PROPERTIES CXX_EXTENSIONS OFF)
target_link_libraries(utils PUBLIC SEAL::seal)

add_executable(main_noopt main_noopt.cpp)
target_link_libraries(main_noopt PRIVATE gen_he_hamming_distance_noopt)
target_link_libraries(main_noopt PRIVATE utils)

add_executable(main_opt main_opt.cpp)
target_link_libraries(main_opt PRIVATE gen_he_hamming_distance)
target_link_libraries(main_opt PRIVATE utils)
