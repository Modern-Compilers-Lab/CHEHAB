set(FHECO_SOURCE_FILES ${FHECO_SOURCE_FILES}
    ${CMAKE_CURRENT_LIST_DIR}/common.cpp
    ${CMAKE_CURRENT_LIST_DIR}/fold_op_gen_matcher.cpp
    ${CMAKE_CURRENT_LIST_DIR}/op_gen_matcher.cpp
    ${CMAKE_CURRENT_LIST_DIR}/op_gen_op_code.cpp
    ${CMAKE_CURRENT_LIST_DIR}/ops_overloads.cpp
    ${CMAKE_CURRENT_LIST_DIR}/rule.cpp
    ${CMAKE_CURRENT_LIST_DIR}/ruleset.cpp
    ${CMAKE_CURRENT_LIST_DIR}/subst.cpp
    ${CMAKE_CURRENT_LIST_DIR}/term_matcher.cpp
    ${CMAKE_CURRENT_LIST_DIR}/term_op_code.cpp
    ${CMAKE_CURRENT_LIST_DIR}/trs.cpp
)

install(
    FILES
        ${CMAKE_CURRENT_LIST_DIR}/common.hpp
        ${CMAKE_CURRENT_LIST_DIR}/fold_op_gen_matcher.hpp
        ${CMAKE_CURRENT_LIST_DIR}/op_gen_matcher.hpp
        ${CMAKE_CURRENT_LIST_DIR}/op_gen_op_code.hpp
        ${CMAKE_CURRENT_LIST_DIR}/ops_overloads.hpp
        ${CMAKE_CURRENT_LIST_DIR}/rule.hpp
        ${CMAKE_CURRENT_LIST_DIR}/ruleset.hpp
        ${CMAKE_CURRENT_LIST_DIR}/subst.hpp
        ${CMAKE_CURRENT_LIST_DIR}/term_matcher.hpp
        ${CMAKE_CURRENT_LIST_DIR}/term_op_code.hpp
        ${CMAKE_CURRENT_LIST_DIR}/trs.hpp

    DESTINATION
        ${FHECO_INCLUDES_INSTALL_DIR}/trs
)

set(FHECO_SOURCE_FILES ${FHECO_SOURCE_FILES} PARENT_SCOPE)
