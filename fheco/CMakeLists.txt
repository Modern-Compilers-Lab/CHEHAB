set(SRC_DSL src/dsl)
set(SRC_IR src/ir)
set(SRC_OPTIMIZER src/optimizer)
set(SRC_PARAMS_SELECTOR src/params_selector)
set(SRC_TRANSLATOR src/translator)
set(SRC_UTILS src/utils)

set(CMAKE_CXX_STANDARD 17)

set(HEADERS_INSTALL_DIR include/fhecompiler)

set(FHECOMPILER_STATIC_INSTALL_DIR lib)

add_library(fhecompiler fhecompiler.cpp)

target_include_directories(${PROJECT_NAME} PUBLIC src/ir PUBLIC src/dsl PUBLIC src/optimizer PUBLIC src/params_selector PUBLIC src/translator src/utils)

target_sources(fhecompiler PRIVATE
  ${SRC_DSL}/ciphertext.cpp
  ${SRC_DSL}/plaintext.cpp
  ${SRC_DSL}/scalar.cpp
  ${SRC_DSL}/ops_overloads.cpp
  ${SRC_IR}/dag.cpp
  ${SRC_IR}/program.cpp
  ${SRC_IR}/term.cpp
  ${SRC_OPTIMIZER}/cse_pass.cpp
  ${SRC_OPTIMIZER}/preprocessor.cpp
  ${SRC_OPTIMIZER}/relin_pass.cpp
  ${SRC_PARAMS_SELECTOR}/params_selector.cpp
  ${SRC_TRANSLATOR}/translator.cpp
)

set_target_properties(fhecompiler PROPERTIES PUBLIC_HEADER fhecompiler.hpp)
set_target_properties(fhecompiler PROPERTIES PRIVATE_HEADER
  ${SRC_DSL}/ciphertext.hpp 
  ${SRC_DSL}/datatypes_const.hpp
  ${SRC_DSL}/datatypes_util.hpp 
  ${SRC_DSL}/ops_overloads.hpp 
  ${SRC_DSL}/plaintext.hpp
  ${SRC_DSL}/scalar.hpp
  ${SRC_IR}/dag.hpp
  ${SRC_IR}/ir_const.hpp
  ${SRC_IR}/program.hpp
  ${SRC_IR}/term.hpp
  ${SRC_OPTIMIZER}/optimizer.hpp
  ${SRC_OPTIMIZER}/cse_pass.hpp
  ${SRC_OPTIMIZER}/preprocessor.hpp
  ${SRC_OPTIMIZER}/relin_pass.hpp
  ${SRC_PARAMS_SELECTOR}/encryption_parameters.hpp
  ${SRC_PARAMS_SELECTOR}/params_selector.hpp
  ${SRC_TRANSLATOR}/translator.hpp
  ${SRC_TRANSLATOR}/translator_const.hpp
  ${SRC_UTILS}/fhecompiler_const.hpp
)

install(TARGETS fhecompiler LIBRARY DESTINATION ${FHECOMPILER_STATIC_INSTALL_DIR} PUBLIC_HEADER DESTINATION ${HEADERS_INSTALL_DIR} PRIVATE_HEADER DESTINATION ${HEADERS_INSTALL_DIR})
