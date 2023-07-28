cmake_minimum_required(VERSION 3.22.1)

project(SEAL_PROGRAMS VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

add_library(ml ml.cpp)

find_package(SEAL 4.0.0 EXACT REQUIRED)

add_executable(cryptonets cryptonets.cpp)

target_link_libraries(ml PRIVATE SEAL::seal)

target_link_libraries(cryptonets PRIVATE ml PRIVATE SEAL::seal)