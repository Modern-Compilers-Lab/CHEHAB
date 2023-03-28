cmake_minimum_required(VERSION 3.13)

project(HammingDistance VERSION 1.0 LANGUAGES CXX)

add_executable(hamming_distance hamming_distance.cpp)
target_compile_features(hamming_distance PUBLIC cxx_std_17)
set_target_properties(hamming_distance PROPERTIES CXX_EXTENSIONS OFF)

target_link_libraries(hamming_distance PRIVATE fhecompiler)
