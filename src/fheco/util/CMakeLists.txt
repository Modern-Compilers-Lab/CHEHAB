set(FHECO_SOURCE_FILES ${FHECO_SOURCE_FILES}
    ${CMAKE_CURRENT_LIST_DIR}/clear_data_eval.cpp
    ${CMAKE_CURRENT_LIST_DIR}/common.cpp
    ${CMAKE_CURRENT_LIST_DIR}/draw_ir.cpp
    ${CMAKE_CURRENT_LIST_DIR}/eval_on_clear.cpp
    ${CMAKE_CURRENT_LIST_DIR}/expr_printer.cpp
    ${CMAKE_CURRENT_LIST_DIR}/quantifier.cpp
)

install(
    FILES
        ${CMAKE_CURRENT_LIST_DIR}/clear_data_eval.hpp
        ${CMAKE_CURRENT_LIST_DIR}/common.hpp
        ${CMAKE_CURRENT_LIST_DIR}/draw_ir_constants.hpp
        ${CMAKE_CURRENT_LIST_DIR}/draw_ir.hpp
        ${CMAKE_CURRENT_LIST_DIR}/eval_on_clear.hpp
        ${CMAKE_CURRENT_LIST_DIR}/expr_printer.hpp
        ${CMAKE_CURRENT_LIST_DIR}/quantifier.hpp

    DESTINATION
        ${FHECO_INCLUDES_INSTALL_DIR}/util
)

set(FHECO_SOURCE_FILES ${FHECO_SOURCE_FILES} PARENT_SCOPE)
