cmake_minimum_required(VERSION 3.13)

project(GXKernel VERSION 1.0 LANGUAGES CXX)

add_executable(gx_kernel gx_kernel.cpp)
target_compile_features(gx_kernel PUBLIC cxx_std_17)
set_target_properties(gx_kernel PROPERTIES CXX_EXTENSIONS OFF)

target_link_libraries(gx_kernel PRIVATE fhecompiler)
