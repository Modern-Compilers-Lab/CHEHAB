project (UFHE VERSION 1.0 LANGUAGES CXX)

add_library(${PROJECT_NAME}
    ${PROJECT_SOURCE_DIR}/src/ufhe/seal_backend/coeffmodulus.cpp
    ${PROJECT_SOURCE_DIR}/src/ufhe/config.cpp
    ${PROJECT_SOURCE_DIR}/src/ufhe/coeffmodulus.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC src)

find_package(SEAL 4.0.0 EXACT REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC SEAL::seal)
