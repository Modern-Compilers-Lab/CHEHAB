add_executable(sobel_checks sobel_checks.cpp)
target_link_libraries(sobel_checks PRIVATE fheco)

add_executable(sobel sobel.cpp)
target_link_libraries(sobel PRIVATE fheco)

