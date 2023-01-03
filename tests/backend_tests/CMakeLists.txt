add_executable(backendtests)

set(BACKEND_SOURCES
  ${CMAKE_CURRENT_LIST_DIR}/backend_tester.cpp
  ${CMAKE_CURRENT_LIST_DIR}/test12.cpp
)

set(INCLUDE_HEADERS_DIR /usr/local/include)

target_sources(backendtests PRIVATE
  ${BACKEND_SOURCES}
)

include_directories(${INCLUDE_HEADERS_DIR})

find_package(SEAL 4.0.0 EXACT REQUIRED)

target_link_libraries(backendtests ufhe SEAL::seal)
