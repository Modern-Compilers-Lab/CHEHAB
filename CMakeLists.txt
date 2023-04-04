cmake_minimum_required(VERSION 3.13)

project(FHECompiler VERSION 1.0 LANGUAGES CXX)

add_subdirectory(fheco)
add_subdirectory(benchmarks/box_blur)
add_subdirectory(benchmarks/box_blur_porcupine)
add_subdirectory(benchmarks/gx_kernel)
add_subdirectory(benchmarks/gx_kernel_porcupine)
add_subdirectory(benchmarks/gy_kernel)
add_subdirectory(benchmarks/sobel)
add_subdirectory(benchmarks/hamming_distance)
add_subdirectory(benchmarks/matrix_mul)
