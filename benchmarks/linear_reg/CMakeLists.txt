add_executable(linear_reg_checks linear_reg_checks.cpp)
target_link_libraries(linear_reg_checks PRIVATE fheco)

add_executable(linear_reg linear_reg.cpp)
target_link_libraries(linear_reg PRIVATE fheco)
