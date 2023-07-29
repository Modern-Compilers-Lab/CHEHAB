set(CMAKE_CXX_STANDARD 17)

add_executable(box_blur box_blur.cpp)

include_directories(/usr/local/include/FHECO)

target_link_libraries(box_blur PRIVATE fheco)
