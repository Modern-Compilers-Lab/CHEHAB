#include "scalar.hpp"
#include "compiler.hpp"
#include "datatypes_util.hpp"
#include "term.hpp"
#include <memory>

namespace fhecompiler
{

size_t Scalar::scalar_id = 0;

using Ptr = std::shared_ptr<ir::Term>;

void Scalar::set_new_label()
{
  this->set_label(datatype::sc_label_prefix + std::to_string(Scalar::scalar_id++));
}

Scalar::Scalar(std::int64_t _data) : data(_data), label(datatype::sc_label_prefix + std::to_string(Scalar::scalar_id++))
{
  Compiler::get_active()->insert_node_in_dataflow<Scalar>(*this);
  Compiler::get_active()->insert_entry_in_constants_table(
    {this->label, {ir::ConstantTableEntryType::constant, {label, _data}}});
}

Scalar::Scalar(std::uint64_t _data)
  : data(_data), label(datatype::sc_label_prefix + std::to_string(Scalar::scalar_id++))
{
  Compiler::get_active()->insert_node_in_dataflow<Scalar>(*this);
  Compiler::get_active()->insert_entry_in_constants_table(
    {this->label, {ir::ConstantTableEntryType::constant, {label, _data}}});
}

/*
Scalar::Scalar(uint64_t _data) : data(_data), label(datatype::sc_label_prefix + std::to_string(Scalar::scalar_id++))
{
  Compiler::get_active()->insert_node_in_dataflow<Scalar>(*this);
  Compiler::get_active()->insert_entry_in_constants_table({this->label, {ir::ConstantTableEntryType::constant, {label,
_data}}});
}
*/

Scalar::Scalar(double _data) : data(_data), label(datatype::sc_label_prefix + std::to_string(Scalar::scalar_id++))
{
  Compiler::get_active()->insert_node_in_dataflow<Scalar>(*this);
  Compiler::get_active()->insert_entry_in_constants_table(
    {this->label, {ir::ConstantTableEntryType::constant, {label, _data}}});
}

Scalar::Scalar() : label(datatype::sc_label_prefix + std::to_string(Scalar::scalar_id++))
{
  Compiler::get_active()->insert_node_in_dataflow<Scalar>(*this);
  Compiler::get_active()->insert_entry_in_constants_table(
    {this->label, {ir::ConstantTableEntryType::constant, {label, data}}});
}

Scalar::Scalar(const std::string &tag) : label(datatype::sc_label_prefix + std::to_string(Scalar::scalar_id++))
{

  // we are expecting from the user to provide a tag for input
  operate_in_constants_table(this->label, tag, VarType::constant);
}

Scalar &Scalar::operator=(const Scalar &sc_copy)
{
  return operate_assignement<Scalar>(*this, sc_copy, ir::TermType::scalar);
}

Scalar::Scalar(const Scalar &sc_copy) : label(datatype::sc_label_prefix + std::to_string(scalar_id++))
{

  auto sc_copy_node_ptr = Compiler::get_active()->insert_node_in_dataflow<Scalar>(sc_copy);
  Compiler::get_active()->insert_operation_node_in_dataflow(
    ir::OpCode::assign, {sc_copy_node_ptr}, this->label, ir::TermType::scalar);
  // std::cout << this->label << " = " << sc_copy.get_label() << "\n";
}

Scalar &Scalar::operator+=(const Scalar &rhs)
{

  compound_operate<Scalar, Scalar>(*this, rhs, ir::OpCode::add, ir::TermType::scalar);
  return *this;
}

Scalar &Scalar::operator*=(const Scalar &rhs)
{
  compound_operate<Scalar, Scalar>(*this, rhs, ir::OpCode::mul, ir::TermType::scalar);
  return *this;
}

Scalar &Scalar::operator-=(const Scalar &rhs)
{
  compound_operate<Scalar, Scalar>(*this, rhs, ir::OpCode::sub, ir::TermType::scalar);
  return *this;
}

Scalar Scalar::operator-()
{
  return operate_unary<Scalar, Scalar>(*this, ir::OpCode::negate, ir::TermType::scalar);
}

Scalar operator+(const Scalar &lhs, const Scalar &rhs)
{
  return operate_binary<Scalar, Scalar, Scalar>(lhs, rhs, ir::OpCode::add, ir::TermType::scalar);
}

Scalar operator*(const Scalar &lhs, const Scalar &rhs)
{
  return operate_binary<Scalar, Scalar, Scalar>(lhs, rhs, ir::OpCode::mul, ir::TermType::scalar);
}

Scalar operator-(const Scalar &lhs, const Scalar &rhs)
{

  return operate_binary<Scalar, Scalar, Scalar>(lhs, rhs, ir::OpCode::sub, ir::TermType::scalar);
}

Scalar operator-(const Scalar &rhs)
{
  return operate_unary<Scalar, Scalar>(rhs, ir::OpCode::negate, ir::TermType::scalar);
}

} // namespace fhecompiler
