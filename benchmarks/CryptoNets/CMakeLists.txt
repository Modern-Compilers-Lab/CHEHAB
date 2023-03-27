cmake_minimum_required(VERSION 3.22.1)

project(CryptoNets VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

add_executable(CryptoNets CryptoNets.cpp)

target_link_libraries(CryptoNets fhecompiler)
