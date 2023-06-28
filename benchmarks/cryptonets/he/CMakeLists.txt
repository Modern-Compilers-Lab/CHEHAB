cmake_minimum_required(VERSION 3.13)

project(HECryptonets VERSION 1.0 LANGUAGES CXX)

SET(SEAL_HEADERS "" CACHE STRING "SEAl headers path")
SET(SEAL_OBJECT "" CACHE STRING "SEAl object path")

add_library(gen_he_cryptonets_noopt STATIC gen_he_cryptonets_noopt.cpp)
target_compile_features(gen_he_cryptonets_noopt PUBLIC cxx_std_17)
set_target_properties(gen_he_cryptonets_noopt PROPERTIES CXX_EXTENSIONS OFF)
target_include_directories(gen_he_cryptonets_noopt PUBLIC ${SEAL_HEADERS})
target_link_libraries(gen_he_cryptonets_noopt PUBLIC ${SEAL_OBJECT})

add_library(gen_he_cryptonets_opt STATIC gen_he_cryptonets_opt.cpp)
target_compile_features(gen_he_cryptonets_opt PUBLIC cxx_std_17)
set_target_properties(gen_he_cryptonets_opt PROPERTIES CXX_EXTENSIONS OFF)
target_include_directories(gen_he_cryptonets_opt PUBLIC ${SEAL_HEADERS})
target_link_libraries(gen_he_cryptonets_opt PUBLIC ${SEAL_OBJECT})

add_library(utils STATIC utils.cpp)
target_compile_features(utils PUBLIC cxx_std_17)
set_target_properties(utils PROPERTIES CXX_EXTENSIONS OFF)
target_include_directories(utils PUBLIC ${SEAL_HEADERS})
target_link_libraries(utils PUBLIC ${SEAL_OBJECT})

add_executable(main_noopt main_noopt.cpp)
target_link_libraries(main_noopt PRIVATE gen_he_cryptonets_noopt)
target_link_libraries(main_noopt PRIVATE utils)

add_executable(main_opt main_opt.cpp)
target_link_libraries(main_opt PRIVATE gen_he_cryptonets_opt)
target_link_libraries(main_opt PRIVATE utils)
