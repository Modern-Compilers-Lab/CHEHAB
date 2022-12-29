cmake_minimum_required(VERSION 3.22.1)

project(testinputs VERSION 1.0 LANGUAGES CXX)

include_directories(/usr/local/include)

add_executable(test1 test1.cpp)

target_link_libraries(test1 fhecompiler)
