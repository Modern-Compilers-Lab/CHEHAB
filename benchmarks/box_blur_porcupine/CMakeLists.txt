set(CMAKE_CXX_STANDARD 17)

include_directories(/usr/local/include/FHECO)

add_executable(box_blur_porcupine_checks box_blur_checks.cpp)
target_link_libraries(box_blur_porcupine_checks PRIVATE fheco)

add_executable(box_blur_porcupine box_blur.cpp)
target_link_libraries(box_blur_porcupine PRIVATE fheco)

