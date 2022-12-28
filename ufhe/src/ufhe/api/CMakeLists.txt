set(UFHEAPI_HEADER_FILES ${UFHEAPI_HEADER_FILES}
    ${CMAKE_CURRENT_LIST_DIR}/abstract_type.hpp
    ${CMAKE_CURRENT_LIST_DIR}/api.hpp
    ${CMAKE_CURRENT_LIST_DIR}/backend_type.hpp
    ${CMAKE_CURRENT_LIST_DIR}/batch_encoder.hpp
    ${CMAKE_CURRENT_LIST_DIR}/ciphertext.hpp
    ${CMAKE_CURRENT_LIST_DIR}/coeff_modulus.hpp
    ${CMAKE_CURRENT_LIST_DIR}/decryptor.hpp
    ${CMAKE_CURRENT_LIST_DIR}/encryption_context.hpp
    ${CMAKE_CURRENT_LIST_DIR}/encryption_params.hpp
    ${CMAKE_CURRENT_LIST_DIR}/encryptor.hpp
    ${CMAKE_CURRENT_LIST_DIR}/evaluator.hpp
    ${CMAKE_CURRENT_LIST_DIR}/galois_keys.hpp
    ${CMAKE_CURRENT_LIST_DIR}/implementation_level.hpp
    ${CMAKE_CURRENT_LIST_DIR}/key_generator.hpp
    ${CMAKE_CURRENT_LIST_DIR}/modulus.hpp
    ${CMAKE_CURRENT_LIST_DIR}/plaintext.hpp
    ${CMAKE_CURRENT_LIST_DIR}/public_key.hpp
    ${CMAKE_CURRENT_LIST_DIR}/relin_keys.hpp
    ${CMAKE_CURRENT_LIST_DIR}/scheme.hpp
    ${CMAKE_CURRENT_LIST_DIR}/secret_key.hpp
)

set(UFHEAPI_HEADER_FILES ${UFHEAPI_HEADER_FILES} PARENT_SCOPE)
