set(FHECO_SOURCE_FILES ${FHECO_SOURCE_FILES}
    ${CMAKE_CURRENT_LIST_DIR}/cse_commut.cpp
    ${CMAKE_CURRENT_LIST_DIR}/get_rotation_keys_steps.cpp
    ${CMAKE_CURRENT_LIST_DIR}/insert_relin.cpp
    ${CMAKE_CURRENT_LIST_DIR}/prepare_code_gen.cpp
    ${CMAKE_CURRENT_LIST_DIR}/reduce_rotation_keys.cpp
    ${CMAKE_CURRENT_LIST_DIR}/scalar_mul_to_add.cpp
)

install(
    FILES
    ${CMAKE_CURRENT_LIST_DIR}/cse_commut.hpp
    ${CMAKE_CURRENT_LIST_DIR}/get_rotation_keys_steps.hpp
    ${CMAKE_CURRENT_LIST_DIR}/insert_relin.hpp
    ${CMAKE_CURRENT_LIST_DIR}/passes.hpp
    ${CMAKE_CURRENT_LIST_DIR}/prepare_code_gen.hpp
    ${CMAKE_CURRENT_LIST_DIR}/reduce_rotation_keys.hpp
    ${CMAKE_CURRENT_LIST_DIR}/scalar_mul_to_add.hpp

    DESTINATION
        ${FHECO_INCLUDES_INSTALL_DIR}/passes
)

set(FHECO_SOURCE_FILES ${FHECO_SOURCE_FILES} PARENT_SCOPE)
