add_executable(gx_kernel_checks gx_kernel_checks.cpp)
target_link_libraries(gx_kernel_checks PRIVATE fheco)

add_executable(gx_kernel gx_kernel.cpp)
target_link_libraries(gx_kernel PRIVATE fheco)

