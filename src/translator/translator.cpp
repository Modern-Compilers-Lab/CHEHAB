#include "translator.hpp"
#include "program.hpp"
#include "translator_const.hpp"

namespace translator
{

Translator::Translator(ir::Program *prgm) : program(prgm) {}

void Translator::translate_constant_table_entry(const ir::ConstantTableEntry &table_entry) const
{
  /*
  basically in this function we need to generate a decalartion or a definition in C++ for a given entry.
  the entry could be representing an input, output, a constant (with raw data) or some temp variable with a tag
  */
}

void Translator::translate_binary_operation(
  const Ptr &term_ptr, const std::optional<std::reference_wrapper<ir::ConstantTableEntry>> &table_entry_opt) const
{}
void Translator::translate_nary_operation(
  const Ptr &term_ptr, const std::optional<std::reference_wrapper<ir::ConstantTableEntry>> &table_entry_opt) const
{}
void Translator::translate_unary_operation(
  const Ptr &term_ptr, const std::optional<std::reference_wrapper<ir::ConstantTableEntry>> &table_entry_opt) const
{}

void Translator::translate_term(const Ptr &term) const
{

  auto constant_table_entry_opt = program->get_entry_form_constants_table(term->get_label());

  if (constant_table_entry_opt != std::nullopt)
  {
    const ir::ConstantTableEntry &constant_table_entry = *constant_table_entry_opt;
    translate_constant_table_entry(constant_table_entry);
  }

  // we need to tranlsate the operation node
  if (term->get_operands() != std::nullopt)
  {
    const std::vector<Ptr> &operands = *(term->get_operands());

    if (operands.size() == 1)
    {
      translate_unary_operation(term, constant_table_entry_opt);
    }
    else if (operands.size() == 2)
    {
      translate_binary_operation(term, constant_table_entry_opt);
    }
    else
    {
      translate_nary_operation(term, constant_table_entry_opt);
    }
  }
}

void Translator::translate(std::ofstream &translation_os) const {}

} // namespace translator
