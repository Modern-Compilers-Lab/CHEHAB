project (UFHE VERSION 1.0 LANGUAGES CXX)

add_library(${PROJECT_NAME}
    ${PROJECT_SOURCE_DIR}/src/ufhe/seal_backend/coeff_modulus.cpp
    ${PROJECT_SOURCE_DIR}/src/ufhe/config.cpp
    ${PROJECT_SOURCE_DIR}/src/ufhe/coeff_modulus.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC src)

find_package(SEAL 4.0.0 EXACT REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC SEAL::seal)
