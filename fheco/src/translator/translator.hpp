#pragma once

#include "encryption_parameters.hpp"
#include "program.hpp"
#include "term.hpp"
#include "translator_const.hpp"
#include <memory>
#include <ostream>
#include <set>
#include <string>

namespace translator
{
class Translator
{
private:
  std::shared_ptr<ir::Program> program;

  EncodingWriter encoding_writer;
  EncryptionWriter encryption_writer;
  EvaluationWriter evaluation_writer;
  ContextWriter context_writer;
  std::unordered_map<ir::Term::Ptr, size_t> size_of;
  std::unordered_map<ir::Term::Ptr, size_t> func_id_by_root;
  std::unordered_map<ir::Term::Ptr, std::vector<std::string>> outputs;
  std::unordered_map<ir::Term::Ptr, std::unordered_set<size_t>> scopes_by_node;

  std::unordered_map<std::string, std::string> label_in_destination_code; // we don't have to create a map but it is
                                                                          // just more elegant with the map

  void translate_binary_operation(const ir::Term::Ptr &term_ptr, std::ostream &os);
  void translate_nary_operation(const ir::Term::Ptr &term_ptr, std::ostream &os);
  void translate_unary_operation(const ir::Term::Ptr &term_ptr, std::ostream &os);

  void translate_constant_table_entry(
    const std::string &identifier, ir::ConstantTableEntry &table_entry, ir::TermType term_type, std::ostream &os);

  void translate_term(const ir::Term::Ptr &term_ptr, std::ostream &os);

  std::string get_identifier(const ir::Term::Ptr &term_ptr);

  std::string get_tag(const ir::Term::Ptr &term_ptr);

  // std::string get_identifier(const std::string &label) const;

  void convert_to_inplace(const ir::Term::Ptr &node_ptr);

  bool is_convertable_to_inplace(const ir::Term::Ptr &node_ptr);

  void convert_to_square(const ir::Term::Ptr &node_ptr);

  void convert_to_inplace_pass();

  // void compact_assignement(const ir::Term::Ptr &node_ptr);

public:
  Translator(
    const std::shared_ptr<ir::Program> &prgm, fhecompiler::SecurityLevel sec_level,
    const param_selector::EncryptionParameters &params, bool uses_mod_switch)
    : program(prgm), encoding_writer(params.poly_modulus_degree(), encoder_identifier, encode_literal),
      encryption_writer(encryptor_identifier, encrypt_literal),
      evaluation_writer(evaluator_type_literal, evaluator_identifier, context_identifier),
      context_writer(params, program->get_encryption_scheme(), uses_mod_switch, sec_level)
  {}

  void generate_function_signature(std::ostream &os) const;

  void generate_key_generator(std::ostream &os) const;

  void generate_rotation_keys(std::ostream &os) const;

  void write_assign_operation(
    std::ostream &os, const std::string &lhs_id, const std::string &rhs_id, ir::TermType type);

  void write_input(const std::string &identifier, const std::string &tag, ir::TermType type, std::ostream &os);
  void write_output(const std::string &identifier, const std::string &tag, ir::TermType type, std::ostream &os);

  void write_rotations_steps_getter(const std::set<int> &steps, std::ostream &os);

  /*
    This function deduce the OpCode to be used by the translator in order to generate the instruction for the backend.an
    example of deduction is that the function can deduce add_plain from add if one of the operands is a ciphertext and
    the other one is a plaintext/scalar.
  */
  ir::OpCode deduce_opcode_to_generate(const ir::Term::Ptr &node) const;

  void translate_program(
    std::ostream &source_os, std::ostream &header_os, const std::string &header_name, size_t threshold,
    const std::set<int> &rotation_keys_steps);

  void translate_program(std::ostream &os, size_t threshold);
  /*
    every change that needs to be done on IR node before code generation happens in fix_ir_instruction
  */
  // void fix_ir_instruction(const ir::Term::Ptr &node);

  // void fix_ir_instructions_pass();

  std::string get_output_identifier(const std::string &output_label);

  std::string get_function_identifier(size_t func_id);

  std::string get_static_object_identifier(const ir::Term::Ptr &node_ptr);

  void write_as_a_static_object(const ir::Term::Ptr &node, std::ostream &os);

  void generate_function_from_nodes(
    std::ostream &os, const std::vector<ir::Term::Ptr> &nodes, bool void_func = false, const std::string &func_id = "");

  void define_class_in_header(std::ostream &header_os);

  void write_static_object_from_function_call(const ir::Term::Ptr &node, std::ostream &os);
};
} // namespace translator
