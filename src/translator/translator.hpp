#include "program.hpp"
#include "term.hpp"
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

  void translate_binary_operation(
    const Ptr &term_ptr, const std::optional<std::reference_wrapper<ir::ConstantTableEntry>> &table_entry_opt) const;
  void translate_nary_operation(
    const Ptr &term_ptr, const std::optional<std::reference_wrapper<ir::ConstantTableEntry>> &table_entry_opt) const;
  void translate_unary_operation(
    const Ptr &term_ptr, const std::optional<std::reference_wrapper<ir::ConstantTableEntry>> &table_entry_opt) const;

  void translate_constant_table_entry(const ir::ConstantTableEntry &table_entry) const;

  void translate_term(const Ptr &term_ptr) const;

public:
  Translator(ir::Program *prgm);
  ~Translator() {}

  void translate(std::ofstream &translation_os) const;
};

} // namespace translator
