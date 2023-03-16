cmake_minimum_required(VERSION 3.13)

project(xdepth VERSION 1.0 LANGUAGES CXX)

add_executable(${PROJECT_NAME} main.cpp)

find_package(SEAL 4.0.0 EXACT REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE SEAL::seal)
