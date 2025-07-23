cmake_minimum_required(VERSION 3.13)

project(FHECo VERSION 1.0 LANGUAGES CXX)

include(GNUInstallDirs)

option(FHECO_LOGGING "FHECO_LOGGING" OFF)
message(STATUS "FHECO_LOGGING: ${FHECO_LOGGING}")


set(FHECO_INCLUDES_INSTALL_DIR ${CMAKE_INSTALL_INCLUDEDIR}/FHECO/fheco)

add_subdirectory(src/fheco)
add_library(fheco STATIC ${FHECO_SOURCE_FILES})
target_include_directories(fheco PUBLIC src)
if(FHECO_LOGGING)
    target_compile_definitions(fheco PRIVATE FHECO_LOGGING)
endif()
target_compile_features(fheco PUBLIC cxx_std_17)
set_target_properties(fheco PROPERTIES CXX_EXTENSIONS OFF)
install(TARGETS fheco ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR})

add_subdirectory(benchmarks/box_blur)
# add_subdirectory(benchmarks/cryptonets)
add_subdirectory(benchmarks/sort)
add_subdirectory(benchmarks/max)
add_subdirectory(benchmarks/dot_product)
add_subdirectory(benchmarks/gx_kernel) 
add_subdirectory(benchmarks/gy_kernel) 
add_subdirectory(benchmarks/sobel) 
add_subdirectory(benchmarks/roberts_cross) 
add_subdirectory(benchmarks/hamming_dist)  
add_subdirectory(benchmarks/l2_distance)
add_subdirectory(benchmarks/lin_reg)
add_subdirectory(benchmarks/matrix_mul)
add_subdirectory(benchmarks/poly_reg)
add_subdirectory(benchmarks/polynomials_coyote)