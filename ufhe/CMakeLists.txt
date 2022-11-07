cmake_minimum_required(VERSION 3.13)

project (UFHE VERSION 1.0 LANGUAGES CXX)

set(UFHE_SOURCE_FILES "")

add_subdirectory(src/ufhe/seal_backend)
add_subdirectory(src/ufhe)

add_library(ufhe STATIC ${UFHE_SOURCE_FILES})

target_include_directories(ufhe PUBLIC src)

find_package(SEAL 4.0.0 EXACT REQUIRED)
target_link_libraries(ufhe PUBLIC SEAL::seal)

add_subdirectory(examples/rotations)
add_subdirectory(examples/simple_addition)
