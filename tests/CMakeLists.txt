cmake_minimum_required(VERSION 3.22.1)

project(FHECOtests VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(frontend_tests)

add_subdirectory(backend_tests)
