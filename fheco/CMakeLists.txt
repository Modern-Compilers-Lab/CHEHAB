set(SRC_ROOT src)
set(SRC_DSL src/dsl)
set(SRC_IR src/ir)
set(SRC_PASSES src/passes)
set(SRC_PARAMS_SELECTOR src/param_selector)
set(SRC_TRANSLATOR src/translator)
set(SRC_UTILS src/utils)
set(SRC_TRS src/trs)

set(FHECOMPILER_SOURCE_FILES
  ${SRC_DSL}/compiler.cpp
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
  ${SRC_UTILS}/clear_data_evaluator.cpp
  ${SRC_UTILS}/evaluate_on_clear.cpp
  ${SRC_UTILS}/common.cpp
  ${SRC_TRS}/reduction_order/utils.cpp
  ${SRC_TRS}/reduction_order/component_orders.cpp
  ${SRC_TRS}/ruleset.cpp
  ${SRC_PASSES}/normalize_pass.cpp
  # ${SRC_PASSES}/additional_passes.cpp
)

add_library(fhecompiler STATIC ${FHECOMPILER_SOURCE_FILES})
target_compile_features(fhecompiler PUBLIC cxx_std_17)
set_target_properties(fhecompiler PROPERTIES CXX_EXTENSIONS OFF)

target_include_directories(fhecompiler
  PUBLIC ${SRC_ROOT}
  PUBLIC ${SRC_IR}
  PUBLIC ${SRC_DSL}
  PUBLIC ${SRC_PASSES}
  PUBLIC ${SRC_PARAMS_SELECTOR}
  PUBLIC ${SRC_TRANSLATOR}
  PUBLIC ${SRC_UTILS}
  PUBLIC ${SRC_TRS}
  PUBLIC ${SRC_TRS}/reduction_order)

include(GNUInstallDirs)

set(FHECOMPILER_INCLUDES_INSTALL_DIR ${CMAKE_INSTALL_INCLUDEDIR}/FHECOMPILER)

install(FILES
  src/fhecompiler.hpp
  ${SRC_DSL}/compiler.hpp
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
  # ${SRC_PASSES}/additional_passes.hpp
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
  ${SRC_UTILS}/clear_data_evaluator.hpp
  ${SRC_UTILS}/evaluate_on_clear.hpp
  ${SRC_UTILS}/common.hpp
  DESTINATION ${FHECOMPILER_INCLUDES_INSTALL_DIR}
)

install(TARGETS fhecompiler ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR})
