cmake_minimum_required(VERSION 3.22.1)

project(testoutputs VERSION 1.0 LANGUAGES CXX)

include_directories(/usr/local/include)

find_package(SEAL 4.0.0 EXACT REQUIRED)

add_executable(test1 test1.cpp)

target_link_libraries(test1 PRIVATE ufhe PRIVATE SEAL::seal)
