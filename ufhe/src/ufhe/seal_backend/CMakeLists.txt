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

set(UFHESEALBACCKED_HEADER_FILES ${UFHESEALBACCKED_HEADER_FILES}
    ${CMAKE_CURRENT_LIST_DIR}/coeff_modulus.hpp
    ${CMAKE_CURRENT_LIST_DIR}/batch_encoder.hpp
    ${CMAKE_CURRENT_LIST_DIR}/ciphertext.hpp
    ${CMAKE_CURRENT_LIST_DIR}/coeff_modulus.hpp
    ${CMAKE_CURRENT_LIST_DIR}/decryptor.hpp
    ${CMAKE_CURRENT_LIST_DIR}/encryption_context.hpp
    ${CMAKE_CURRENT_LIST_DIR}/encryption_params.hpp
    ${CMAKE_CURRENT_LIST_DIR}/encryptor.hpp
    ${CMAKE_CURRENT_LIST_DIR}/evaluator.hpp
    ${CMAKE_CURRENT_LIST_DIR}/galois_keys.hpp
    ${CMAKE_CURRENT_LIST_DIR}/key_generator.hpp
    ${CMAKE_CURRENT_LIST_DIR}/modulus.hpp
    ${CMAKE_CURRENT_LIST_DIR}/plaintext.hpp
    ${CMAKE_CURRENT_LIST_DIR}/public_key.hpp
    ${CMAKE_CURRENT_LIST_DIR}/relin_keys.hpp
    ${CMAKE_CURRENT_LIST_DIR}/scheme.hpp
    ${CMAKE_CURRENT_LIST_DIR}/secret_key.hpp
    ${CMAKE_CURRENT_LIST_DIR}/secret_key.hpp
    ${CMAKE_CURRENT_LIST_DIR}/seal_backend.hpp
)

set(UFHE_SOURCE_FILES ${UFHE_SOURCE_FILES} PARENT_SCOPE)
set(UFHESEALBACCKED_HEADER_FILES ${UFHESEALBACCKED_HEADER_FILES} PARENT_SCOPE)

