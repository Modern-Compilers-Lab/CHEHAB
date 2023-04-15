add_executable(cryptonets cryptonets.cpp)
add_library(ml ml.cpp)

target_link_libraries(ml fhecompiler)
target_link_libraries(cryptonets ml fhecompiler)
