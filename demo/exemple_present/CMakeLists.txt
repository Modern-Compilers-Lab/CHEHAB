cmake_minimum_required(VERSION 3.13)

project(ExemplePresent VERSION 1.0 LANGUAGES CXX)

SET(FHECO_HEADERS "" CACHE STRING "FHECO headers path")
SET(FHECO_OBJECT "" CACHE STRING "FHECO object path")

add_executable(exemple_present exemple_present.cpp)
target_compile_features(exemple_present PRIVATE cxx_std_17)
set_target_properties(exemple_present PROPERTIES CXX_EXTENSIONS OFF)
target_include_directories(exemple_present PRIVATE ${FHECO_HEADERS})
target_link_libraries(exemple_present PRIVATE ${FHECO_OBJECT})
