cmake_minimum_required(VERSION 3.22.1)

set(CMAKE_C_COMPILER "gcc")
set(CMAKE_CXX_COMPILER "g++")

project(FHECO_PROJECT LANGUAGES C CXX)

add_executable(${PROJECT_NAME} benchmarks/test1.cpp)

include_directories(${PROJECT_NAME} src/ir src/user_datatypes src/fhecompiler)

add_library(user_datatypes src/user_datatypes/ciphertext.cpp src/user_datatypes/plaintext.cpp src/user_datatypes/scalar.cpp) 
add_library(ir src/ir/program.cpp src/ir/term.cpp src/ir/dag.cpp)


target_link_libraries(${PROJECT_NAME} user_datatypes)
target_link_libraries(${PROJECT_NAME} ir)