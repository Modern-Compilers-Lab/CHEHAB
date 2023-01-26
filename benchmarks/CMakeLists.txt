cmake_minimum_required(VERSION 3.22.1)

add_executable(matrix_mul)

set(SOURCES
  ${CMAKE_CURRENT_LIST_DIR}/matrix_mul.cpp
)

set(INCLUDE_HEADERS_DIR /usr/local/include)

target_sources(matrix_mul PRIVATE
  ${SOURCES}
)

include_directories(${INCLUDE_HEADERS_DIR})

target_link_libraries(matrix_mul fhecompiler)
