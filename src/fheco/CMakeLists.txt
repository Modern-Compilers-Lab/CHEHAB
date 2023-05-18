set(FHECO_SOURCE_FILES ${FHECO_SOURCE_FILES})

add_subdirectory(dsl)
add_subdirectory(ir)
add_subdirectory(passes)
add_subdirectory(trs)
add_subdirectory(util)

install(
    FILES
        ${CMAKE_CURRENT_LIST_DIR}/fheco.hpp
    DESTINATION
        ${FHECO_INCLUDES_INSTALL_DIR}
)

set(FHECO_SOURCE_FILES ${FHECO_SOURCE_FILES} PARENT_SCOPE)
