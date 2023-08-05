add_executable(poly_reg_checks poly_reg_checks.cpp)
target_link_libraries(poly_reg_checks PRIVATE fheco)

add_executable(poly_reg poly_reg.cpp)
target_link_libraries(poly_reg PRIVATE fheco)
