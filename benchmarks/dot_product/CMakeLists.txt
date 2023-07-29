add_executable(dot_product dot_product.cpp)

include_directories(/usr/local/include)

target_link_libraries(dot_product PRIVATE fheco)
