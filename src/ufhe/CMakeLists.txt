project (UFHE VERSION 1.0 LANGUAGES CXX)

add_library(${PROJECT_NAME}
    ${CMAKE_CURRENT_LIST_DIR}/seal_backend/coeff_modulus.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC ../)

find_package(SEAL 4.0.0 EXACT REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC SEAL::seal)
