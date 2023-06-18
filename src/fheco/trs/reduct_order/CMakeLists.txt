set(FHECO_SOURCE_FILES ${FHECO_SOURCE_FILES}
    ${CMAKE_CURRENT_LIST_DIR}/check_ruleset.cpp
    ${CMAKE_CURRENT_LIST_DIR}/common.cpp
    ${CMAKE_CURRENT_LIST_DIR}/compon_orders.cpp 
)

install(
    FILES
        ${CMAKE_CURRENT_LIST_DIR}/check_ruleset.hpp
        ${CMAKE_CURRENT_LIST_DIR}/common.hpp
        ${CMAKE_CURRENT_LIST_DIR}/compon_orders.hpp 

    DESTINATION
        ${FHECO_INCLUDES_INSTALL_DIR}/trs/reduct_order
)

set(FHECO_SOURCE_FILES ${FHECO_SOURCE_FILES} PARENT_SCOPE)
