cmake_minimum_required(VERSION 3.13)

project(seal_bfv VERSION 1.0 LANGUAGES CXX)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)
add_executable(sealbfv wrapper_seal_bfv.cpp)

find_package(SEAL 4.0.0 EXACT REQUIRED)
target_link_libraries(sealbfv PRIVATE SEAL::seal)
