cmake_minimum_required(VERSION 3.13)

project(HECryptonets VERSION 1.0 LANGUAGES CXX)

find_package(SEAL 4.1.1 EXACT REQUIRED)

add_library(gen_he_cryptonets_noopt STATIC gen_he_cryptonets_noopt.cpp)
target_compile_features(gen_he_cryptonets_noopt PUBLIC cxx_std_17)
set_target_properties(gen_he_cryptonets_noopt PROPERTIES CXX_EXTENSIONS OFF)
target_link_libraries(gen_he_cryptonets_noopt PUBLIC SEAL::seal)

add_library(gen_he_cryptonets STATIC gen_he_cryptonets.cpp)
target_compile_features(gen_he_cryptonets PUBLIC cxx_std_17)
set_target_properties(gen_he_cryptonets PROPERTIES CXX_EXTENSIONS OFF)
target_link_libraries(gen_he_cryptonets PUBLIC SEAL::seal)

add_library(utils STATIC utils.cpp)
target_compile_features(utils PUBLIC cxx_std_17)
set_target_properties(utils PROPERTIES CXX_EXTENSIONS OFF)
target_link_libraries(utils PUBLIC SEAL::seal)

add_executable(main_noopt main_noopt.cpp)
target_link_libraries(main_noopt PRIVATE gen_he_cryptonets)
target_link_libraries(main_noopt PRIVATE utils)

add_executable(main_opt main_opt.cpp)
target_link_libraries(main_opt PRIVATE gen_he_cryptonets)
target_link_libraries(main_opt PRIVATE gen_he_cryptonets)

