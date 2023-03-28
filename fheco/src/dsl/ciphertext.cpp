#include "ciphertext.hpp"
#include "compiler.hpp"
#include "datatypes_const.hpp"
#include "datatypes_util.hpp"

namespace fhecompiler
{
size_t Ciphertext::ciphertext_id = 0;

Ciphertext::Ciphertext(const std::string &tag)
  : label(datatype::ct_label_prefix + std::to_string(Ciphertext::ciphertext_id++))
{
  Compiler::get_active()->insert_node_in_dataflow(*this);
  operate_in_constants_table(this->label, tag, VarType::input);
  Compiler::init_input(label, tag);
}

void Ciphertext::set_output(const std::string &tag)
{
  operate_in_constants_table(this->label, tag, VarType::output);
  Compiler::set_output(label, tag);
}

void Ciphertext::set_new_label()
{
  this->set_label(datatype::ct_label_prefix + std::to_string(Ciphertext::ciphertext_id++));
}

std::string Ciphertext::get_term_tag()
{
  auto table_entry = Compiler::get_active()->get_entry_form_constants_table(this->label);
  if (table_entry != std::nullopt)
  {
    ir::ConstantTableEntry table_entry_dereferenced = *table_entry;
    return table_entry_dereferenced.get_entry_value().tag;
  }
  else
    return "";
}

bool Ciphertext::is_output() const
{
  return (Compiler::get_active()->type_of(this->label) == ir::ConstantTableEntryType::output);
}
} // namespace fhecompiler
