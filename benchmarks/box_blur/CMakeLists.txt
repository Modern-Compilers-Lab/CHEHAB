add_executable(box_blur_checks box_blur_checks.cpp)
target_link_libraries(box_blur_checks PRIVATE fheco)

add_executable(box_blur box_blur.cpp)
target_link_libraries(box_blur PRIVATE fheco)

