set(CMAKE_CXX_STANDARD 17)

include_directories(/usr/local/include/FHECO)

add_executable(test test.cpp)
target_link_libraries(test PRIVATE fheco)

