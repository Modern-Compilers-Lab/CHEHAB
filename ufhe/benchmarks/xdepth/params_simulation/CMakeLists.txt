cmake_minimum_required(VERSION 3.13)

project(params_simulation VERSION 1.0 LANGUAGES CXX)

add_library(params_simulation STATIC params_simulation.cpp)

find_package(SEAL 4.0.0 EXACT REQUIRED)
target_link_libraries(params_simulation PUBLIC SEAL::seal)

add_executable(test_params_simulation main.cpp)
target_link_libraries(test_params_simulation PRIVATE params_simulation)
