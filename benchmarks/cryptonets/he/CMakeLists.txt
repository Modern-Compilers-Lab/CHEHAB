cmake_minimum_required(VERSION 3.13)

project(HECryptonets VERSION 1.0 LANGUAGES CXX)

add_library(utils STATIC utils.cpp)
target_compile_features(utils PUBLIC cxx_std_17)
set_target_properties(utils PROPERTIES CXX_EXTENSIONS OFF)

find_package(SEAL 4.1.1 EXACT REQUIRED)
target_link_libraries(utils PUBLIC SEAL::seal)

add_executable(he_cryptonets main.cpp)
target_link_libraries(he_cryptonets PRIVATE utils)
