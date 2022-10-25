#pragma once

#include "encryption_context.hpp"
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
  params_selector::EncryptionContext *context;

  void translate_binary_operation(
    const Ptr &term_ptr, std::optional<std::reference_wrapper<ir::ConstantTableEntry>> &table_entry_opt,
    std::ofstream &os, const Evaluator &evaluator) const;
  void translate_nary_operation(
    const Ptr &term_ptr, std::optional<std::reference_wrapper<ir::ConstantTableEntry>> &table_entry_opt,
    std::ofstream &os, const Evaluator &evaluator) const;
  void translate_unary_operation(
    const Ptr &term_ptr, std::optional<std::reference_wrapper<ir::ConstantTableEntry>> &table_entry_opt,
    std::ofstream &os, const Evaluator &evaluator) const;

  void translate_constant_table_entry(
    ir::ConstantTableEntry &table_entry, ir::TermType term_type, std::ofstream &os, Encoder &encoder) const;

  void translate_term(const Ptr &term_ptr, std::ofstream &os, const Evaluator &evaluator, Encoder &encoder) const;

  std::string get_identifier(const Ptr &term_ptr) const;

public:
  Translator(ir::Program *prgm, params_selector::EncryptionContext *ctxt) : program(prgm), context(ctxt) {}
  void generate_function_signature(std::ofstream &os) const;
  ~Translator() {}

  void translate(std::ofstream &os) const;
};

} // namespace translator
