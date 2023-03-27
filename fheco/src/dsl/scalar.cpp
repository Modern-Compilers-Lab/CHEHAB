#include "scalar.hpp"
#include "datatypes_util.hpp"
#include "program.hpp"
#include "term.hpp"
#include <memory>

using namespace datatype;

extern ir::Program *program;

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
  program->insert_node_in_dataflow<Scalar>(*this);
  program->insert_entry_in_constants_table({this->label, {ir::ConstantTableEntryType::constant, {label, _data}}});
}

Scalar::Scalar(std::uint64_t _data)
  : data(_data), label(datatype::sc_label_prefix + std::to_string(Scalar::scalar_id++))
{
  if (program == nullptr)
    throw(program_not_init_msg);

  program->insert_node_in_dataflow<Scalar>(*this);
  program->insert_entry_in_constants_table({this->label, {ir::ConstantTableEntryType::constant, {label, _data}}});
}

/*
Scalar::Scalar(uint64_t _data) : data(_data), label(datatype::sc_label_prefix + std::to_string(Scalar::scalar_id++))
{
  program->insert_node_in_dataflow<Scalar>(*this);
  program->insert_entry_in_constants_table({this->label, {ir::ConstantTableEntryType::constant, {label, _data}}});
}
*/

Scalar::Scalar(double _data) : data(_data), label(datatype::sc_label_prefix + std::to_string(Scalar::scalar_id++))
{
  if (program == nullptr)
    throw(program_not_init_msg);

  program->insert_node_in_dataflow<Scalar>(*this);
  program->insert_entry_in_constants_table({this->label, {ir::ConstantTableEntryType::constant, {label, _data}}});
}

Scalar::Scalar() : label(datatype::sc_label_prefix + std::to_string(Scalar::scalar_id++))
{
  if (program == nullptr)
    throw(program_not_init_msg);

  program->insert_node_in_dataflow<Scalar>(*this);
  program->insert_entry_in_constants_table({this->label, {ir::ConstantTableEntryType::constant, {label, data}}});
}

Scalar::Scalar(const std::string &tag) : label(datatype::sc_label_prefix + std::to_string(Scalar::scalar_id++))
{

  // we are expecting from the user to provide a tag for input
  operate_in_constants_table(this->label, tag, VarType::constant);
}

Scalar &Scalar::operator=(const Scalar &sc_copy)
{
  return operate_copy_assignement<Scalar>(*this, sc_copy, ir::TermType::scalar);
}

Scalar &Scalar::operator=(Scalar &&sc_move)
{
  return operate_move_assignement<Scalar>(*this, std::move(sc_move), ir::TermType::scalar);
}

Scalar::Scalar(const Scalar &sc_copy) : label(datatype::sc_label_prefix + std::to_string(scalar_id++))
{
  operate_copy<Scalar>(*this, sc_copy, ir::TermType::scalar);
}

Scalar::Scalar(Scalar &&sc_move) : label(datatype::sc_label_prefix + std::to_string(scalar_id++))
{
  operate_move<Scalar>(*this, std::move(sc_move), ir::TermType::scalar);
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
  return 0 - *this;
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
  return 0 - rhs;
}

} // namespace fhecompiler
