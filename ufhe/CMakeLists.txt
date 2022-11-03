project (UFHE VERSION 1.0 LANGUAGES CXX)

add_library(${PROJECT_NAME}
    ${PROJECT_SOURCE_DIR}/src/ufhe/seal_backend/coeff_modulus.cpp
    ${PROJECT_SOURCE_DIR}/src/ufhe/coeff_modulus.cpp
    ${PROJECT_SOURCE_DIR}/src/ufhe/batch_encoder.cpp
    ${PROJECT_SOURCE_DIR}/src/ufhe/ciphertext.cpp
    ${PROJECT_SOURCE_DIR}/src/ufhe/coeff_modulus.cpp
    ${PROJECT_SOURCE_DIR}/src/ufhe/config.cpp
    ${PROJECT_SOURCE_DIR}/src/ufhe/decryptor.cpp
    ${PROJECT_SOURCE_DIR}/src/ufhe/encryption_context.cpp
    ${PROJECT_SOURCE_DIR}/src/ufhe/encryption_params.cpp
    ${PROJECT_SOURCE_DIR}/src/ufhe/encryptor.cpp
    ${PROJECT_SOURCE_DIR}/src/ufhe/evaluator.cpp
    ${PROJECT_SOURCE_DIR}/src/ufhe/galois_keys.cpp
    ${PROJECT_SOURCE_DIR}/src/ufhe/key_generator.cpp
    ${PROJECT_SOURCE_DIR}/src/ufhe/modulus.cpp
    ${PROJECT_SOURCE_DIR}/src/ufhe/plaintext.cpp
    ${PROJECT_SOURCE_DIR}/src/ufhe/public_key.cpp
    ${PROJECT_SOURCE_DIR}/src/ufhe/relin_keys.cpp
    ${PROJECT_SOURCE_DIR}/src/ufhe/scheme.cpp
    ${PROJECT_SOURCE_DIR}/src/ufhe/secret_key.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC src)

find_package(SEAL 4.0.0 EXACT REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC SEAL::seal)
