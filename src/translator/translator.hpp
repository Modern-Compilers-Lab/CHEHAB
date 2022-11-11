#pragma once

#include "encryption_parameters.hpp"
#include "program.hpp"
#include "term.hpp"
#include "translator_const.hpp"
#include <fstream>
#include <iostream>
#include <string>

namespace translator
{

using Ptr = std::shared_ptr<ir::Term>;

class Translator
{

private:
  ir::Program *program;
  params_selector::EncryptionParameters *encryption_parameters;

  EncodingWriter encoding_writer;
  EncryptionWriter encryption_writer;
  EvaluationWriter evaluation_writer;
  ContextWriter context_writer;

  void translate_binary_operation(
    const Ptr &term_ptr, std::optional<std::reference_wrapper<ir::ConstantTableEntry>> &table_entry_opt,
    std::ofstream &os);
  void translate_nary_operation(
    const Ptr &term_ptr, std::optional<std::reference_wrapper<ir::ConstantTableEntry>> &table_entry_opt,
    std::ofstream &os);
  void translate_unary_operation(
    const Ptr &term_ptr, std::optional<std::reference_wrapper<ir::ConstantTableEntry>> &table_entry_opt,
    std::ofstream &os);

  void translate_constant_table_entry(ir::ConstantTableEntry &table_entry, ir::TermType term_type, std::ofstream &os);

  void translate_term(const Ptr &term_ptr, std::ofstream &os);

  std::string get_identifier(const Ptr &term_ptr) const;

  void convert_operation_to_inplace(const ir::Term::Ptr &node_ptr);

public:
  Translator(ir::Program *prgm, params_selector::EncryptionParameters *params)
    : program(prgm), encryption_parameters(params),
      encoding_writer(encoder_type_literal, encoder_type_identifier, context_identifier, encode_literal),
      encryption_writer(
        encryptor_type_literal, encryptor_type_identifier, encrypt_literal, public_key_identifier, context_identifier),
      evaluation_writer(evaluator_type_literal, evaluator_identifier, context_identifier),
      context_writer(this->encryption_parameters, program->get_encryption_scheme())
  {}

  void generate_function_signature(std::ofstream &os) const;
  ~Translator() {}

  void write_assign_operation(
    std::ofstream &os, const std::string &lhs_id, const std::string &rhs_id, ir::TermType type);

  void write_input(const std::string &input_identifier, ir::TermType type, std::ostream &os);
  void write_output(const std::string &output_identifier, ir::TermType type, std::ostream &os);

  void translate(std::ofstream &os);
};

} // namespace translator
