#include "plaintext.hpp"
#include "compiler.hpp"
#include "datatypes_const.hpp"
#include "datatypes_util.hpp"

namespace fhecompiler
{

size_t Plaintext::plaintext_id = 0;

Plaintext::Plaintext(const std::vector<std::int64_t> &data)
  : label(datatype::pt_label_prefix + std::to_string(Plaintext::plaintext_id++))
{
  Compiler::add_const_node_value(label, data);
  Compiler::get_active()->insert_node_in_dataflow<Plaintext>(*this);
  Compiler::get_active()->insert_entry_in_constants_table(
    {this->label, {ir::ConstantTableEntryType::constant, {label, data}}});
}

Plaintext::Plaintext(const std::vector<std::uint64_t> &data)
  : label(datatype::pt_label_prefix + std::to_string(Plaintext::plaintext_id++))
{
  Compiler::add_const_node_value(label, data);
  Compiler::get_active()->insert_node_in_dataflow<Plaintext>(*this);
  Compiler::get_active()->insert_entry_in_constants_table(
    {this->label, {ir::ConstantTableEntryType::constant, {label, data}}});
}

Plaintext::Plaintext(const std::vector<double> &data)
  : label(datatype::pt_label_prefix + std::to_string(Plaintext::plaintext_id++))
{
  Compiler::get_active()->insert_node_in_dataflow<Plaintext>(*this);
  Compiler::get_active()->insert_entry_in_constants_table(
    {this->label, {ir::ConstantTableEntryType::constant, {label, data}}});
}

Plaintext::Plaintext(const std::string &tag)
  : label(datatype::pt_label_prefix + std::to_string(Plaintext::plaintext_id++))
{
  Compiler::init_input(label, tag);
  Compiler::get_active()->insert_node_in_dataflow(*this);
  operate_in_constants_table(this->label, tag, VarType::input);
}

void Plaintext::set_output(const std::string &tag)
{
  operate_in_constants_table(this->label, tag, VarType::output);
}

void Plaintext::set_new_label()
{
  this->set_label(datatype::pt_label_prefix + std::to_string(Plaintext::plaintext_id++));
}
} // namespace fhecompiler
