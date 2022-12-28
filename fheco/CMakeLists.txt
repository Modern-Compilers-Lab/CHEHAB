set(SRC_DSL src/dsl)
set(SRC_IR src/ir)
set(SRC_OPTIMIZER src/optimizer)
set(SRC_PARAMS_SELECTOR src/params_selector)
set(SRC_TRANSLATOR src/translator)
set(SRC_UTILS src/utils)

set(CMAKE_CXX_STANDARD 17)

add_library(fhecompiler fhecompiler.cpp)

target_include_directories(${PROJECT_NAME} PUBLIC src/ir PUBLIC src/dsl PUBLIC src/optimizer PUBLIC src/params_selector PUBLIC src/translator src/utils)

target_sources(fhecompiler 
PRIVATE ${SRC_DSL}/ciphertext.cpp
PRIVATE ${SRC_DSL}/plaintext.cpp
PRIVATE ${SRC_DSL}/scalar.cpp
PRIVATE ${SRC_DSL}/ops_overloads.cpp
PRIVATE ${SRC_IR}/dag.cpp
PRIVATE ${SRC_IR}/program.cpp
PRIVATE ${SRC_IR}/term.cpp
PRIVATE ${SRC_OPTIMIZER}/cse_pass.cpp
PRIVATE ${SRC_OPTIMIZER}/preprocessor.cpp
PRIVATE ${SRC_OPTIMIZER}/relin_pass.cpp
PRIVATE ${SRC_PARAMS_SELECTOR}/params_selector.cpp
PRIVATE ${SRC_TRANSLATOR}/translator.cpp
)

set_target_properties(fhecompiler PROPERTIES PUBLIC_HEADER fhecompiler.hpp)
set_target_properties(fhecompiler PROPERTIES
PRIVATE_HEADER ${SRC_DSL}/ciphertext.hpp 
PRIVATE_HEADER ${SRC_DSL}/datatypes_const.hpp
PRIVATE_HEADER ${SRC_DSL}/datatypes_util.hpp 
PRIVATE_HEADER ${SRC_DSL}/ops_overloads.hpp 
PRIVATE_HEADER ${SRC_DSL}/plaintext.hpp
PRIVATE_HEADER ${SRC_DSL}/scalar.hpp
PRIVATE_HEADER ${SRC_IR}/dag.hpp
PRIVATE_HEADER ${SRC_IR}/ir_const.hpp
PRIVATE_HEADER ${SRC_IR}/program.hpp
PRIVATE_HEADER ${SRC_IR}/term.hpp
PRIVATE_HEADER ${SRC_OPTIMIZER}/optimizer.hpp
PRIVATE_HEADER ${SRC_OPTIMIZER}/cse_pass.hpp
PRIVATE_HEADER ${SRC_OPTIMIZER}/preprocessor.hpp
PRIVATE_HEADER ${SRC_OPTIMIZER}/relin_pass.hpp
PRIVATE_HEADER ${SRC_PARAMS_SELECTOR}/encryption_parameters.hpp
PRIVATE_HEADER ${SRC_PARAMS_SELECTOR}/params_selector.hpp
PRIVATE_HEADER ${SRC_TRANSLATOR}/translator.hpp
PRIVATE_HEADER ${SRC_TRANSLATOR}/translator_const.hpp
PRIVATE_HEADER ${SRC_UTILS}/fhecompiler_const.hpp
)

install(TARGETS fhecompiler LIBRARY DESTINATION lib PUBLIC_HEADER DESTINATION include/fhecompiler PRIVATE_HEADER DESTINATION include/fhecompiler)
