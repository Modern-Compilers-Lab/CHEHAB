cmake_minimum_required(VERSION 3.22.1)

set(CMAKE_C_COMPILER "gcc")
set(CMAKE_CXX_COMPILER "g++")

project(FHECO_PROJECT LANGUAGES C CXX)

add_executable(${PROJECT_NAME} benchmarks/test1.cpp)

include_directories(${PROJECT_NAME} src/ir src/dsl src/fhecompiler src/translator)

add_library(dsl src/dsl/ciphertext.cpp src/dsl/plaintext.cpp src/dsl/scalar.cpp) 
add_library(ir src/ir/program.cpp src/ir/term.cpp src/ir/dag.cpp)
add_library(translator src/translator/translator.cpp)


target_link_libraries(${PROJECT_NAME} dsl)
target_link_libraries(${PROJECT_NAME} ir)
target_link_libraries(${PROJECT_NAME} translator)
