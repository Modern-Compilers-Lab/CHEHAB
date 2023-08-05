add_executable(l2_distance_checks l2_distance_checks.cpp)
target_link_libraries(l2_distance_checks PRIVATE fheco)

add_executable(l2_distance l2_distance.cpp)
target_link_libraries(l2_distance PRIVATE fheco)

