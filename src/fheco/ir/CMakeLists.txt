set(FHECO_SOURCE_FILES ${FHECO_SOURCE_FILES}
    ${CMAKE_CURRENT_LIST_DIR}/common.cpp
    ${CMAKE_CURRENT_LIST_DIR}/dag.cpp
    ${CMAKE_CURRENT_LIST_DIR}/function.cpp
    ${CMAKE_CURRENT_LIST_DIR}/op_code.cpp
    ${CMAKE_CURRENT_LIST_DIR}/term.cpp
)

install(
    FILES
        ${CMAKE_CURRENT_LIST_DIR}/common.hpp
        ${CMAKE_CURRENT_LIST_DIR}/dag.hpp
        ${CMAKE_CURRENT_LIST_DIR}/function.hpp
        ${CMAKE_CURRENT_LIST_DIR}/op_code.hpp
        ${CMAKE_CURRENT_LIST_DIR}/term.hpp
    DESTINATION
        ${FHECO_INCLUDES_INSTALL_DIR}/ir
)

set(FHECO_SOURCE_FILES ${FHECO_SOURCE_FILES} PARENT_SCOPE)
