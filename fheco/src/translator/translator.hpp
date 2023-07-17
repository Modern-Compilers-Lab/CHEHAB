#pragma once

#include "encryption_parameters.hpp"
#include "program.hpp"
#include "term.hpp"
#include "translator_const.hpp"
#include <fstream>
#include <iostream>
#include <set>
#include <string>

namespace translator
{

using Ptr = std::shared_ptr<ir::Term>;

class Translator
{

  /*
    we create scopes and then later we sort the scopes based on dependencies
  */

private:
  ir::Program *program;
  params_selector::EncryptionParameters *encryption_parameters;

  std::unordered_map<size_t, std::set<size_t>> scopes_graph;
  static size_t scope_id;
  std::unordered_map<ir::Program::Ptr, size_t> created_in;
  std::unordered_map<ir::Program::Ptr, std::set<size_t>> accessed_in;
  std::unordered_map<size_t, std::vector<ir::Program::Ptr>> scope_nodes_by_id;
  std::vector<size_t> scopes_ids_sorted;
  std::unordered_set<ir::Program::Ptr> generated_shareds;
  std::unordered_map<ir::Program::Ptr, std::unordered_multiset<std::string>> parents_copy;
  std::unordered_map<ir::Program::Ptr, std::string> current_label;

  std::unordered_map<ir::TermType, std::vector<std::string>> free_global_objs;
  /*(scope, list of free objects in the scope)*/
  std::unordered_map<ir::TermType, std::unordered_map<size_t, std::vector<std::string>>> free_local_objs;

  EncodingWriter encoding_writer;
  EncryptionWriter encryption_writer;
  EvaluationWriter evaluation_writer;
  ContextWriter context_writer;

  void translate_binary_operation(const Ptr &term_ptr, std::ofstream &os, size_t scope_id, bool is_new_object = true);
  void translate_nary_operation(const Ptr &term_ptr, std::ofstream &os, size_t scope_id);
  void translate_unary_operation(const Ptr &term_ptr, std::ofstream &os, size_t scope_id, bool is_new_object = true);

  void translate_constant_table_entry(
    const ir::Program::Ptr &term, ir::ConstantTableEntry &table_entry, ir::TermType term_type, std::ofstream &os,
    bool is_new_object = true);

  void translate_term(const Ptr &term_ptr, std::ofstream &os, size_t scope_id);

  void translate_term_in_existing(const Ptr &term_ptr, std::ofstream &os);

  std::string get_identifier(const Ptr &term_ptr);

  std::string get_identifier(const std::string &label);

  std::string get_shared_identifier(const Ptr &term);

  std::string get_shared_identifier(const std::string &label, ir::TermType type);

  std::string get_input_identifier(const ir::Program::Ptr &term);

  // void compact_assignement(const ir::Term::Ptr &node_ptr);

public:
  Translator(ir::Program *prgm, params_selector::EncryptionParameters *params)
    : program(prgm), encryption_parameters(params),
      encoding_writer(
        program->get_encryption_scheme() == fhecompiler::Scheme::ckks ? ckks_encoder_type_literal
                                                                      : bv_encoder_type_literal,
        encoder_type_identifier, context_identifier, encode_literal),
      encryption_writer(
        encryptor_type_literal, encryptor_type_identifier, encrypt_literal, public_key_identifier, context_identifier),
      evaluation_writer(evaluator_type_literal, evaluator_identifier, context_identifier),
      context_writer(this->encryption_parameters, program->get_encryption_scheme())
  {}

  ~Translator() {}

  void generate_function_signature(std::ofstream &os) const;

  void generate_function_signature_without_type(std::ofstream &os, const std::string &func_name) const;

  void generate_function_call(std::ofstream &os, const std::string &func_name) const;

  void generate_function_call_class(std::ofstream &os, const std::string &func_name) const;

  void write_scope_function_call(size_t scope_id, std::ofstream &os) const;

  void generate_computation_class(std::ofstream &os_header, std::ofstream &os_source);

  void generate_key_generator(std::ofstream &os) const;

  void generate_rotation_keys(std::ofstream &os) const;

  void generate_class_func_signature(std::ofstream &os, const std::string &func_name) const;

  void generate_constructor_signature(std::ofstream &os, const std::string &class_name) const;

  void generate_main_func_signature(std::ofstream &os, const std::string &func_name) const;

  void define_class_constructor(std::ofstream &os, const std::string &class_name) const;

  void define_main_func(std::ofstream &os, const std::string &func_name) const;

  void write_assign_operation(
    std::ofstream &os, const std::string &lhs_id, const std::string &rhs_id, ir::TermType type);

  void write_input(const std::string &input_identifier, ir::TermType type, std::ostream &os);
  void write_output(
    const std::string &output_tag, const std::string &output_identifier, ir::TermType type, std::ostream &os);

  void write_rotations_steps_getter(const std::vector<int32_t> &steps, std::ostream &os);

  bool is_shared_obj(const ir::Program::Ptr &node, size_t term_scope_id);

  /*
    This function deduce the OpCode to be used by the translator in order to generate the instruction for the backend.an
    example of deduction is that the function can deduce add_plain from add if one of the operands is a ciphertext and
    the other one is a plaintext/scalar.
  */
  ir::OpCode deduce_opcode_to_generate(const Ptr &node) const;

  void translate_program(std::ofstream &os, size_t threshold);

  /*
    every change that needs to be done on IR node before code generation happens in fix_ir_instruction
  */
  void fix_ir_instruction(const ir::Program::Ptr &node);

  void fix_ir_instructions_pass();

  void make_scopes_graph(size_t nodes_count_threshold);

  std::string get_output_identifier(const std::string &output_label, ir::TermType term_type);

  void sort_scopes_ids(size_t scope_id, std::unordered_set<size_t> &visited);

  void generate_scope(size_t scope_id, std::ofstream &os);

  void write_as_shared_object(const ir::Program::Ptr &node, std::ofstream &os);
};

} // namespace translator
