cmake_minimum_required(VERSION 3.13)

project(Cryptonets VERSION 1.0 LANGUAGES CXX)

SET(FHECO_HEADERS "" CACHE STRING "FHECO headers path")
SET(FHECO_OBJECT "" CACHE STRING "FHECO object path")

add_executable(cryptonets_checks cryptonets_checks.cpp)
add_executable(cryptonets cryptonets.cpp)

add_library(ml STATIC ml.cpp)
target_compile_features(ml PUBLIC cxx_std_17)
set_target_properties(ml PROPERTIES CXX_EXTENSIONS OFF)
target_include_directories(ml PUBLIC ${FHECO_HEADERS})
target_link_libraries(ml PUBLIC ${FHECO_OBJECT})

target_include_directories(cryptonets_checks PRIVATE ${FHECO_HEADERS})
target_link_libraries(cryptonets_checks PRIVATE ${FHECO_OBJECT} ml)

target_include_directories(cryptonets PRIVATE ${FHECO_HEADERS})
target_link_libraries(cryptonets PRIVATE ${FHECO_OBJECT} ml)
