add_executable(gy_kernel_checks gy_kernel_checks.cpp)
target_link_libraries(gy_kernel_checks PRIVATE fheco)

add_executable(gy_kernel gy_kernel.cpp)
target_link_libraries(gy_kernel PRIVATE fheco)

