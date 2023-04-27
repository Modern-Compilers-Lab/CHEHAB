set(FHECO_SOURCE_FILES ${FHECO_SOURCE_FILES}
    ${CMAKE_CURRENT_LIST_DIR}/clear_data_evaluator.cpp
    ${CMAKE_CURRENT_LIST_DIR}/common.cpp
    ${CMAKE_CURRENT_LIST_DIR}/draw_ir.cpp
    ${CMAKE_CURRENT_LIST_DIR}/evaluate_on_clear.cpp
    ${CMAKE_CURRENT_LIST_DIR}/quantifier.cpp

)

install(
    FILES
        ${CMAKE_CURRENT_LIST_DIR}/clear_data_evaluator.hpp
        ${CMAKE_CURRENT_LIST_DIR}/common.hpp
        ${CMAKE_CURRENT_LIST_DIR}/draw_ir.hpp
        ${CMAKE_CURRENT_LIST_DIR}/evaluate_on_clear.hpp
        ${CMAKE_CURRENT_LIST_DIR}/quantifier.hpp

    DESTINATION
        ${FHECO_INCLUDES_INSTALL_DIR}/util
)

set(FHECO_SOURCE_FILES ${FHECO_SOURCE_FILES} PARENT_SCOPE)
