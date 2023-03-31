cmake_minimum_required(VERSION 3.22.1)

project(fhecompiler VERSION 1.0 LANGUAGES CXX)

add_subdirectory(third_party/SEAL)

add_subdirectory(fheco)
add_subdirectory(ufhe)
