add_executable(cryptonets cryptonets.cpp)
add_executable(cryptonets_checks cryptonets_checks.cpp)


add_library(ml STATIC ml.cpp)
target_link_libraries(ml PRIVATE fheco)

target_link_libraries(cryptonets PRIVATE fheco ml)
target_link_libraries(cryptonets_checks PRIVATE fheco ml)

