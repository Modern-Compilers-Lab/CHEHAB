cmake_minimum_required(VERSION 3.22.1)

project(CryptoNets VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

add_executable(CryptoNets CryptoNets.cpp)

target_link_libraries(CryptoNets fhecompiler)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/benchmarks/CryptoNets/he)

set(SOURCE_FILE_1 ${CMAKE_SOURCE_DIR}/benchmarks/CryptoNets/he/main.cpp)
set(DESTINATION_FILE_1 ${CMAKE_SOURCE_DIR}/build/benchmarks/CryptoNets/he/main.cpp)
set(SOURCE_FILE_2 ${CMAKE_SOURCE_DIR}/benchmarks/CryptoNets/he/utils.cpp)
set(DESTINATION_FILE_2 ${CMAKE_SOURCE_DIR}/build/benchmarks/CryptoNets/he/utils.cpp)
set(SOURCE_FILE_3 ${CMAKE_SOURCE_DIR}/benchmarks/CryptoNets/he/utils.hpp)
set(DESTINATION_FILE_3 ${CMAKE_SOURCE_DIR}/build/benchmarks/CryptoNets/he/utils.hpp)

add_custom_target(copy_my_file_1CryptoNets ALL
    COMMAND ${CMAKE_COMMAND} -E copy ${SOURCE_FILE_1} ${DESTINATION_FILE_1}
    DEPENDS ${SOURCE_FILE}
    COMMENT "Copying ${SOURCE_FILE_1} to ${DESTINATION_FILE_1}"
)
add_custom_target(copy_my_file_2CryptoNets ALL
    COMMAND ${CMAKE_COMMAND} -E copy ${SOURCE_FILE_2} ${DESTINATION_FILE_2}
    DEPENDS ${SOURCE_FILE}
    COMMENT "Copying ${SOURCE_FILE_2} to ${DESTINATION_FILE_2}"
)
add_custom_target(copy_my_file_3CryptoNets ALL
    COMMAND ${CMAKE_COMMAND} -E copy ${SOURCE_FILE_3} ${DESTINATION_FILE_3}
    DEPENDS ${SOURCE_FILE}
    COMMENT "Copying ${SOURCE_FILE_3} to ${DESTINATION_FILE_3}"
)

set(SOURCE_FILE_4 ${CMAKE_SOURCE_DIR}/benchmarks/Cryptonets/he/CMakeLists.txt)
set(DESTINATION_FILE_4 ${CMAKE_SOURCE_DIR}/build/benchmarks/Cryptonets/he/CMakeLists.txt)
add_custom_target(copy_my_file_4Cryptonets ALL
    COMMAND ${CMAKE_COMMAND} -E copy ${SOURCE_FILE_4} ${DESTINATION_FILE_4}
    DEPENDS ${SOURCE_FILE}
    COMMENT "Copying ${SOURCE_FILE_4} to ${DESTINATION_FILE_4}"
)