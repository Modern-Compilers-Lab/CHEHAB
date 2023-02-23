cmake_minimum_required(VERSION 3.22.1)

project(check_rules VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

add_executable(check_rules check_rules.cpp)

target_link_libraries(check_rules fhecompiler)
