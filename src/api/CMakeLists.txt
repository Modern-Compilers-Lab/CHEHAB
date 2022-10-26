cmake_minimum_required(VERSION 3.13)

project(api VERSION 1.0 LANGUAGES CXX)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)
add_executable(test_api main.cpp)

target_sources(test_api
PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/api.cpp
    ${CMAKE_CURRENT_LIST_DIR}/seal_backend/encryptionparameters.cpp
)

find_package(SEAL 4.0.0 EXACT REQUIRED)
target_link_libraries(test_api PRIVATE SEAL::seal)
