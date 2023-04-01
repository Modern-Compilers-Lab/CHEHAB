#include "scalar.hpp"
#include "compiler.hpp"
#include "datatypes_const.hpp"
#include "datatypes_util.hpp"

namespace fhecompiler
{
size_t Scalar::scalar_id = 0;

Scalar::Scalar(std::int64_t data) : label_(datatype::sc_label_prefix + std::to_string(Scalar::scalar_id++))
{
  Compiler::init_const(data, example_value_);
  Compiler::get_active()->insert_node_in_dataflow<Scalar>(*this);
  Compiler::get_active()->insert_entry_in_constants_table(
    {label_, {ir::ConstantTableEntryType::constant, {label_, data}}});
}

Scalar::Scalar(std::uint64_t data) : label_(datatype::sc_label_prefix + std::to_string(Scalar::scalar_id++))
{
  Compiler::init_const(data, example_value_);
  Compiler::get_active()->insert_node_in_dataflow<Scalar>(*this);
  Compiler::get_active()->insert_entry_in_constants_table(
    {label_, {ir::ConstantTableEntryType::constant, {label_, data}}});
}

void Scalar::set_new_label()
{
  label_ = datatype::sc_label_prefix + std::to_string(Scalar::scalar_id++);
}
} // namespace fhecompiler
