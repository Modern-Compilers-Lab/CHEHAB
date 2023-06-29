cmake_minimum_required(VERSION 3.13)

project(HammingDistPorcupine VERSION 1.0 LANGUAGES CXX)

SET(FHECO_HEADERS "" CACHE STRING "FHECO headers path")
SET(FHECO_OBJECT "" CACHE STRING "FHECO object path")

add_executable(hamming_dist_porcupine_checks hamming_dist_checks.cpp)
target_compile_features(hamming_dist_porcupine_checks PRIVATE cxx_std_17)
set_target_properties(hamming_dist_porcupine_checks PROPERTIES CXX_EXTENSIONS OFF)
target_include_directories(hamming_dist_porcupine_checks PRIVATE ${FHECO_HEADERS})
target_link_libraries(hamming_dist_porcupine_checks PRIVATE ${FHECO_OBJECT})

add_executable(hamming_dist_porcupine hamming_dist.cpp)
target_compile_features(hamming_dist_porcupine PRIVATE cxx_std_17)
set_target_properties(hamming_dist_porcupine PROPERTIES CXX_EXTENSIONS OFF)
target_include_directories(hamming_dist_porcupine PRIVATE ${FHECO_HEADERS})
target_link_libraries(hamming_dist_porcupine PRIVATE ${FHECO_OBJECT})
