add_executable(hamming_dist_checks hamming_dist_checks.cpp)
target_link_libraries(hamming_dist_checks PRIVATE fheco)

add_executable(hamming_dist hamming_dist.cpp)
target_link_libraries(hamming_dist PRIVATE fheco)

