cmake_minimum_required(VERSION 3.22.1)

project(SEAL_PROGRAMS VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

add_library(ml ml.cpp)

add_executable(cryptonets cryptonets.cpp)

target_link_libraries(ml PRIVATE fhecompiler)

target_link_libraries(cryptonets PRIVATE ml fhecompiler)