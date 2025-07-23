set(FHECO_SOURCE_FILES ${FHECO_SOURCE_FILES}
    ${CMAKE_CURRENT_LIST_DIR}/ciphertext.cpp
    ${CMAKE_CURRENT_LIST_DIR}/common.cpp
    ${CMAKE_CURRENT_LIST_DIR}/compiler_helper_functions.cpp
    ${CMAKE_CURRENT_LIST_DIR}/compiler.cpp
    ${CMAKE_CURRENT_LIST_DIR}/ops_overloads.cpp
    ${CMAKE_CURRENT_LIST_DIR}/plaintext.cpp
    ${CMAKE_CURRENT_LIST_DIR}/compiler_simplification.cpp
)

install(
    FILES
        ${CMAKE_CURRENT_LIST_DIR}/ciphertext.hpp
        ${CMAKE_CURRENT_LIST_DIR}/common.hpp
        ${CMAKE_CURRENT_LIST_DIR}/compiler_helper_functions.hpp
        ${CMAKE_CURRENT_LIST_DIR}/compiler.hpp
        ${CMAKE_CURRENT_LIST_DIR}/ops_overloads.hpp
        ${CMAKE_CURRENT_LIST_DIR}/plaintext.hpp
        ${CMAKE_CURRENT_LIST_DIR}/compiler_simplification.hpp
    DESTINATION
        ${FHECO_INCLUDES_INSTALL_DIR}/dsl
)

set(FHECO_SOURCE_FILES ${FHECO_SOURCE_FILES} PARENT_SCOPE)
