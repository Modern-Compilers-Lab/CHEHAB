cmake_minimum_required(VERSION 3.22.1)

add_executable(test1)

set(SOURCES
  ${CMAKE_CURRENT_LIST_DIR}/test1.cpp
)

set(INCLUDE_HEADERS_DIR /usr/local/include)

target_sources(test1 PRIVATE
  ${SOURCES}
)

include_directories(${INCLUDE_HEADERS_DIR})

target_link_libraries(test1 fhecompiler)
