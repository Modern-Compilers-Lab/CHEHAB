add_executable(cryptonets cryptonets.cpp)

add_library(ml STATIC ml.cpp)
target_link_libraries(ml PRIVATE fhecompiler)

target_link_libraries(cryptonets PRIVATE fhecompiler ml)
