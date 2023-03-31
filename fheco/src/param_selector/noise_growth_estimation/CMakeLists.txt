cmake_minimum_required(VERSION 3.13)

project(noise_growth_estimation VERSION 1.0 LANGUAGES CXX)

add_library(utils STATIC utils.cpp)

find_package(SEAL 4.1.1 EXACT REQUIRED)
target_link_libraries(utils PUBLIC SEAL::seal)

add_executable(bfv bfv.cpp)
target_link_libraries(bfv PRIVATE utils)
