add_executable(frontendtests)

set(FRONTEND_SOURCES
  ${CMAKE_CURRENT_LIST_DIR}/frontend_tester.cpp
  ${CMAKE_CURRENT_LIST_DIR}/test1.cpp
  ${CMAKE_CURRENT_LIST_DIR}/test2.cpp
  ${CMAKE_CURRENT_LIST_DIR}/test3.cpp
  ${CMAKE_CURRENT_LIST_DIR}/test4.cpp
  ${CMAKE_CURRENT_LIST_DIR}/test5.cpp
  ${CMAKE_CURRENT_LIST_DIR}/test6.cpp
  ${CMAKE_CURRENT_LIST_DIR}/test7.cpp
  ${CMAKE_CURRENT_LIST_DIR}/test8.cpp
  ${CMAKE_CURRENT_LIST_DIR}/test9.cpp
  ${CMAKE_CURRENT_LIST_DIR}/test10.cpp
  ${CMAKE_CURRENT_LIST_DIR}/test11.cpp
  ${CMAKE_CURRENT_LIST_DIR}/test12.cpp
  ${CMAKE_CURRENT_LIST_DIR}/test13.cpp
  ${CMAKE_CURRENT_LIST_DIR}/test14.cpp
  ${CMAKE_CURRENT_LIST_DIR}/test14.cpp
  ${CMAKE_CURRENT_LIST_DIR}/test15.cpp
  ${CMAKE_CURRENT_LIST_DIR}/test16.cpp
  ${CMAKE_CURRENT_LIST_DIR}/test17.cpp
  ${CMAKE_CURRENT_LIST_DIR}/test18.cpp
  ${CMAKE_CURRENT_LIST_DIR}/test19.cpp
  ${CMAKE_CURRENT_LIST_DIR}/test20.cpp
  ${CMAKE_CURRENT_LIST_DIR}/test21.cpp
  ${CMAKE_CURRENT_LIST_DIR}/test22.cpp


)

set(INCLUDE_HEADERS_DIR /usr/local/include)

target_sources(frontendtests PRIVATE
  ${FRONTEND_SOURCES}
)

include_directories(${INCLUDE_HEADERS_DIR})

target_link_libraries(frontendtests fhecompiler)
