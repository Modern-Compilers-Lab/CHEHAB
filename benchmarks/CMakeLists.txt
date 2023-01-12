cmake_minimum_required(VERSION 3.22.1)

add_executable(gx_kernel)

set(FRONTEND_SOURCES
  ${CMAKE_CURRENT_LIST_DIR}/gx_kernel.cpp
)

set(INCLUDE_HEADERS_DIR /usr/local/include)

target_sources(gx_kernel PRIVATE
  ${FRONTEND_SOURCES}
)

include_directories(${INCLUDE_HEADERS_DIR})

target_link_libraries(gx_kernel fhecompiler)
