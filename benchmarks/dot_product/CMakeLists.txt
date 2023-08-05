add_executable(dot_product_checks dot_product_checks.cpp)
target_link_libraries(dot_product_checks PRIVATE fheco)

add_executable(dot_product dot_product.cpp)
target_link_libraries(dot_product PRIVATE fheco)

