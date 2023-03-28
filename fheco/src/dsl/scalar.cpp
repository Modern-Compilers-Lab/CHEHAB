#include "scalar.hpp"
#include "compiler.hpp"
#include "datatypes_const.hpp"
#include "datatypes_util.hpp"

namespace fhecompiler
{

size_t Scalar::scalar_id = 0;

Scalar::Scalar(std::int64_t data) : label(datatype::sc_label_prefix + std::to_string(Scalar::scalar_id++))
{
  Compiler::add_const_node_value(label, data);
  Compiler::get_active()->insert_node_in_dataflow<Scalar>(*this);
  Compiler::get_active()->insert_entry_in_constants_table(
    {this->label, {ir::ConstantTableEntryType::constant, {label, data}}});
}

Scalar::Scalar(std::uint64_t data) : label(datatype::sc_label_prefix + std::to_string(Scalar::scalar_id++))
{
  Compiler::add_const_node_value(label, data);
  Compiler::get_active()->insert_node_in_dataflow<Scalar>(*this);
  Compiler::get_active()->insert_entry_in_constants_table(
    {this->label, {ir::ConstantTableEntryType::constant, {label, data}}});
}

Scalar::Scalar(double data) : label(datatype::sc_label_prefix + std::to_string(Scalar::scalar_id++))
{
  Compiler::get_active()->insert_node_in_dataflow<Scalar>(*this);
  Compiler::get_active()->insert_entry_in_constants_table(
    {this->label, {ir::ConstantTableEntryType::constant, {label, data}}});
}

void Scalar::set_new_label()
{
  this->set_label(datatype::sc_label_prefix + std::to_string(Scalar::scalar_id++));
}
} // namespace fhecompiler
