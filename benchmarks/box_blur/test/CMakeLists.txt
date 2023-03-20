cmake_minimum_required(VERSION 3.22.1)

project(test_box_blur VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

add_executable(${PROJECT_NAME} main.cpp)

find_package(SEAL 4.1.1 EXACT REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC SEAL::seal)
