set(UFHE_SOURCE_FILES ${UFHE_SOURCE_FILES}
    ${CMAKE_CURRENT_LIST_DIR}/coeff_modulus.cpp
    ${CMAKE_CURRENT_LIST_DIR}/batch_encoder.cpp
    ${CMAKE_CURRENT_LIST_DIR}/ciphertext.cpp
    ${CMAKE_CURRENT_LIST_DIR}/coeff_modulus.cpp
    ${CMAKE_CURRENT_LIST_DIR}/decryptor.cpp
    ${CMAKE_CURRENT_LIST_DIR}/encryption_context.cpp
    ${CMAKE_CURRENT_LIST_DIR}/encryption_params.cpp
    ${CMAKE_CURRENT_LIST_DIR}/encryptor.cpp
    ${CMAKE_CURRENT_LIST_DIR}/evaluator.cpp
    ${CMAKE_CURRENT_LIST_DIR}/galois_keys.cpp
    ${CMAKE_CURRENT_LIST_DIR}/key_generator.cpp
    ${CMAKE_CURRENT_LIST_DIR}/modulus.cpp
    ${CMAKE_CURRENT_LIST_DIR}/plaintext.cpp
    ${CMAKE_CURRENT_LIST_DIR}/public_key.cpp
    ${CMAKE_CURRENT_LIST_DIR}/relin_keys.cpp
    ${CMAKE_CURRENT_LIST_DIR}/scheme.cpp
    ${CMAKE_CURRENT_LIST_DIR}/secret_key.cpp
)

set(UFHE_SOURCE_FILES ${UFHE_SOURCE_FILES} PARENT_SCOPE)
