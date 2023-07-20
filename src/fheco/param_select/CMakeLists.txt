set(FHECO_SOURCE_FILES ${FHECO_SOURCE_FILES}
        ${CMAKE_CURRENT_LIST_DIR}/enc_params.cpp
        # ${CMAKE_CURRENT_LIST_DIR}/param_selec.cpp
)

install(
    FILES
        ${CMAKE_CURRENT_LIST_DIR}/enc_params.hpp
        # ${CMAKE_CURRENT_LIST_DIR}/param_select.hpp
    DESTINATION
        ${FHECO_INCLUDES_INSTALL_DIR}/param_select
)

set(FHECO_SOURCE_FILES ${FHECO_SOURCE_FILES} PARENT_SCOPE)
