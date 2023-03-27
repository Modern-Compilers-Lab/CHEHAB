set(SRC_DSL src/dsl)
set(SRC_IR src/ir)
set(SRC_PASSES src/passes)
set(SRC_PARAMS_SELECTOR src/param_selector)
set(SRC_TRANSLATOR src/translator)
set(SRC_UTILS src/utils)
set(SRC_TRS src/trs)


set(CMAKE_CXX_STANDARD 17)

set(HEADERS_INSTALL_DIR include/fhecompiler)

set(FHECOMPILER_STATIC_INSTALL_DIR lib)

add_library(fhecompiler fhecompiler.cpp)

target_include_directories(${PROJECT_NAME} PUBLIC ${SRC_IR} PUBLIC ${SRC_DSL} PUBLIC ${SRC_PASSES} PUBLIC ${SRC_PARAMS_SELECTOR} PUBLIC ${SRC_TRANSLATOR} PUBLIC ${SRC_UTILS} PUBLIC ${SRC_TRS} PUBLIC ${SRC_TRS}/reduction_order)

target_sources(fhecompiler PRIVATE
  ${SRC_DSL}/ciphertext.cpp
  ${SRC_DSL}/plaintext.cpp
  ${SRC_DSL}/scalar.cpp
  ${SRC_DSL}/ops_overloads.cpp
  ${SRC_IR}/dag.cpp
  ${SRC_IR}/program.cpp
  ${SRC_IR}/term.cpp
  ${SRC_IR}/ir_utils.cpp
  ${SRC_PASSES}/cse_pass.cpp
  ${SRC_PASSES}/relin_pass.cpp
  ${SRC_PASSES}/rotationkeys_select_pass.cpp
  ${SRC_PARAMS_SELECTOR}/param_selector.cpp
  ${SRC_PARAMS_SELECTOR}/encryption_parameters.cpp
  ${SRC_TRANSLATOR}/translator.cpp
  ${SRC_TRS}/trs.cpp
  ${SRC_TRS}/matching_term.cpp
  ${SRC_TRS}/rewrite_rule.cpp
  ${SRC_TRS}/trs_core.cpp
  ${SRC_TRS}/trs_util_functions.cpp
  ${SRC_UTILS}/draw_ir.cpp
  ${SRC_UTILS}/quantify_ir.cpp
  ${SRC_TRS}/reduction_order/utils.cpp
  ${SRC_TRS}/reduction_order/component_orders.cpp
  ${SRC_TRS}/ruleset.cpp
  ${SRC_PASSES}/normalize_pass.cpp
  ${SRC_PASSES}/additional_passes.cpp
)

install(FILES  
  fhecompiler.hpp
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
  ${SRC_IR}/ir_utils.hpp
  ${SRC_PASSES}/cse_pass.hpp
  ${SRC_PASSES}/relin_pass.hpp
  ${SRC_PARAMS_SELECTOR}/encryption_parameters.hpp
  ${SRC_PARAMS_SELECTOR}/param_selector.hpp
  ${SRC_TRANSLATOR}/translator.hpp
  ${SRC_TRANSLATOR}/translator_const.hpp
  ${SRC_UTILS}/fhecompiler_const.hpp
  ${SRC_PASSES}/rotationkeys_select_pass.hpp
  ${SRC_PASSES}/normalize_pass.hpp
  ${SRC_PASSES}/additional_passes.hpp
  ${SRC_TRS}/trs.hpp
  ${SRC_TRS}/matching_term.hpp
  ${SRC_TRS}/rewrite_rule.hpp
  ${SRC_TRS}/trs_const.hpp
  ${SRC_TRS}/reduction_order/utils.hpp
  ${SRC_TRS}/reduction_order/component_orders.hpp
  ${SRC_TRS}/trs_core.hpp
  ${SRC_TRS}/trs_util_functions.hpp
  ${SRC_TRS}/ruleset.hpp
  ${SRC_UTILS}/draw_ir.hpp
  ${SRC_UTILS}/quantify_ir.hpp
  DESTINATION ${HEADERS_INSTALL_DIR}
)

install(TARGETS 
  fhecompiler LIBRARY DESTINATION ${FHECOMPILER_STATIC_INSTALL_DIR} 
)
