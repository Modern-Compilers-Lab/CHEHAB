#include "plaintext.hpp"
#include "compiler.hpp"
#include "datatypes_util.hpp"
#include "term.hpp"
#include <memory>

namespace fhecompiler
{

size_t Plaintext::plaintext_id = 0;

using Ptr = std::shared_ptr<ir::Term>;

void Plaintext::set_new_label()
{
  this->set_label(datatype::pt_label_prefix + std::to_string(Plaintext::plaintext_id++));
}

Plaintext::Plaintext() : label(datatype::pt_label_prefix + std::to_string(Plaintext::plaintext_id++))
{
  operate_in_constants_table(label, label, VarType::temp);
}

Plaintext &Plaintext::operator=(Plaintext &&pt_move)
{
  operate_move<Plaintext>(*this, std::move(pt_move), ir::TermType::plaintext);
  return *this;
}

Plaintext::Plaintext(const std::vector<std::int64_t> &message)
  : label(datatype::pt_label_prefix + std::to_string(Plaintext::plaintext_id++))
{

  /*
  if ((size_t)message.size() > Compiler::get_active()->get_dimension())
    throw("Number of messages in one vector is larger than the expcted value ");
  */

  Compiler::add_const_node_value(label, message);
  Compiler::get_active()->insert_node_in_dataflow<Plaintext>(*this);
  Compiler::get_active()->insert_entry_in_constants_table(
    {this->label, {ir::ConstantTableEntryType::constant, {label, message}}});
}

Plaintext::Plaintext(const std::vector<std::uint64_t> &message)
  : label(datatype::pt_label_prefix + std::to_string(Plaintext::plaintext_id++))
{

  /*
  if ((size_t)message.size() > Compiler::get_active()->get_dimension())
    throw("Number of messages in one vector is larger than the expcted value ");
  */

  Compiler::add_const_node_value(label, message);
  Compiler::get_active()->insert_node_in_dataflow<Plaintext>(*this);
  Compiler::get_active()->insert_entry_in_constants_table(
    {this->label, {ir::ConstantTableEntryType::constant, {label, message}}});
}

Plaintext::Plaintext(const std::vector<double> &message)
  : label(datatype::pt_label_prefix + std::to_string(Plaintext::plaintext_id++))
{
  Compiler::get_active()->insert_node_in_dataflow<Plaintext>(*this);
  Compiler::get_active()->insert_entry_in_constants_table(
    {this->label, {ir::ConstantTableEntryType::constant, {label, message}}});
}

Plaintext::Plaintext(const std::string &tag, VarType var_type)
  : label(datatype::pt_label_prefix + std::to_string(Plaintext::plaintext_id++))
{
  if (var_type == VarType::input)
  {
    Compiler::init_input(label, tag);
    Compiler::get_active()->insert_node_in_dataflow(*this);
  }
  // we are expecting from the user to provide a tag for input
  operate_in_constants_table(this->label, tag, var_type);
}

Plaintext &Plaintext::operator=(const Plaintext &pt_copy)
{
  return operate_assignement<Plaintext>(*this, pt_copy, ir::TermType::plaintext);
}

Plaintext::Plaintext(const Plaintext &pt_copy) : label(datatype::pt_label_prefix + std::to_string(plaintext_id++))
{
  operate_copy<Plaintext>(*this, pt_copy, ir::TermType::plaintext);
  /*
  auto pt_copy_node_ptr = Compiler::get_active()->insert_node_in_dataflow<Plaintext>(pt_copy);
  Compiler::get_active()->insert_operation_node_in_dataflow(ir::OpCode::assign, {pt_copy_node_ptr}, this->label,
  ir::TermType::plaintext);
  */
  // std::cout << this->label << " = " << pt_copy.get_label() << "\n";
}

Plaintext &Plaintext::operator+=(const Plaintext &rhs)
{

  if (is_compile_time_evaluation_possible<Plaintext>(*this, rhs))
  {
    compound_operate<Plaintext, Plaintext>(*this, rhs, ir::OpCode::add, ir::TermType::plaintext);
    return *this;
  }
  else
    throw(datatype::eval_not_supported);
}

Plaintext &Plaintext::operator*=(const Plaintext &rhs)
{
  if (is_compile_time_evaluation_possible<Plaintext>(*this, rhs))
  {
    compound_operate<Plaintext, Plaintext>(*this, rhs, ir::OpCode::mul, ir::TermType::plaintext);
    return *this;
  }
  else
    throw(datatype::eval_not_supported);
}

Plaintext &Plaintext::operator-=(const Plaintext &rhs)
{
  if (is_compile_time_evaluation_possible<Plaintext>(*this, rhs))
  {
    compound_operate<Plaintext, Plaintext>(*this, rhs, ir::OpCode::sub, ir::TermType::plaintext);
    return *this;
  }
  else
    throw(datatype::eval_not_supported);
}

/*
Plaintext Plaintext::operator-()
{
  return operate_unary<Plaintext, Plaintext>(*this, ir::OpCode::negate, ir::TermType::plaintext);
}
*/

Plaintext operator+(const Plaintext &lhs, const Plaintext &rhs)
{
  if (is_compile_time_evaluation_possible<Plaintext>(lhs, rhs))
  {
    return operate_binary<Plaintext, Plaintext, Plaintext>(lhs, rhs, ir::OpCode::add, ir::TermType::plaintext);
  }
  else
    throw(datatype::eval_not_supported);
}

Plaintext operator*(const Plaintext &lhs, const Plaintext &rhs)
{
  if (is_compile_time_evaluation_possible<Plaintext>(lhs, rhs))
  {
    return operate_binary<Plaintext, Plaintext, Plaintext>(lhs, rhs, ir::OpCode::mul, ir::TermType::plaintext);
  }
  else
    throw(datatype::eval_not_supported);
}

Plaintext operator-(const Plaintext &lhs, const Plaintext &rhs)
{
  if (is_compile_time_evaluation_possible<Plaintext>(lhs, rhs))
  {
    return operate_binary<Plaintext, Plaintext, Plaintext>(lhs, rhs, ir::OpCode::sub, ir::TermType::plaintext);
  }
  else
    throw(datatype::eval_not_supported);
}

/*
Plaintext operator-(Plaintext &rhs)
{
  return operate_unary<Plaintext, Plaintext>(rhs, ir::OpCode::negate, ir::TermType::plaintext);
}
*/

} // namespace fhecompiler
