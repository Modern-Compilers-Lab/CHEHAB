set(UFHE_SOURCE_FILES "")

add_subdirectory(src/ufhe/seal_backend)
add_subdirectory(src/ufhe)
add_subdirectory(src/ufhe/api)

add_library(ufhe STATIC ${UFHE_SOURCE_FILES})

target_include_directories(ufhe PUBLIC src)


target_link_libraries(ufhe PUBLIC SEAL::seal)

install(FILES ${UFHE_HEADER_FILES} DESTINATION include/ufhe)
install(FILES ${UFHEAPI_HEADER_FILES} DESTINATION include/ufhe/api)
install(FILES ${UFHESEALBACCKED_HEADER_FILES} DESTINATION include/ufhe/seal_backend)
install(TARGETS ufhe LIBRARY DESTINATION lib)

