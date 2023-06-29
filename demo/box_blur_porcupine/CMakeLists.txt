cmake_minimum_required(VERSION 3.13)

project(BoxBlurPorcupine VERSION 1.0 LANGUAGES CXX)

SET(FHECO_HEADERS "" CACHE STRING "FHECO headers path")
SET(FHECO_OBJECT "" CACHE STRING "FHECO object path")

add_executable(box_blur_porcupine_checks box_blur_checks.cpp)
target_compile_features(box_blur_porcupine_checks PRIVATE cxx_std_17)
set_target_properties(box_blur_porcupine_checks PROPERTIES CXX_EXTENSIONS OFF)
target_include_directories(box_blur_porcupine_checks PRIVATE ${FHECO_HEADERS})
target_link_libraries(box_blur_porcupine_checks PRIVATE ${FHECO_OBJECT})

add_executable(box_blur_porcupine box_blur.cpp)
target_compile_features(box_blur_porcupine PRIVATE cxx_std_17)
set_target_properties(box_blur_porcupine PROPERTIES CXX_EXTENSIONS OFF)
target_include_directories(box_blur_porcupine PRIVATE ${FHECO_HEADERS})
target_link_libraries(box_blur_porcupine PRIVATE ${FHECO_OBJECT})
