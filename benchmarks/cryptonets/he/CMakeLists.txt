cmake_minimum_required(VERSION 3.13)

project(HECryptonets VERSION 1.0 LANGUAGES CXX)

add_library(utils STATIC utils.cpp)
target_compile_features(utils PUBLIC cxx_std_17)
set_target_properties(utils PROPERTIES CXX_EXTENSIONS OFF)

find_package(SEAL 4.1.1 EXACT REQUIRED)
target_link_libraries(utils PUBLIC SEAL::seal)

add_library(gen_he_cryptonets STATIC gen_he_cryptonets.cpp)
target_compile_features(gen_he_cryptonets PUBLIC cxx_std_17)
set_target_properties(gen_he_cryptonets PROPERTIES CXX_EXTENSIONS OFF)

find_package(SEAL 4.1.1 EXACT REQUIRED)
target_link_libraries(gen_he_cryptonets PUBLIC SEAL::seal)

add_executable(he_cryptonets main.cpp)
target_link_libraries(he_cryptonets PRIVATE utils)
target_link_libraries(he_cryptonets PRIVATE gen_he_cryptonets)
