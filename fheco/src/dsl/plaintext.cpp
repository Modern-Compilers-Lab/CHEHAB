#include "plaintext.hpp"
#include "datatypes_util.hpp"
#include "program.hpp"
#include "term.hpp"
#include <memory>

using namespace datatype;

extern ir::Program *program;

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
  operate_move_assignement<Plaintext>(*this, std::move(pt_move), ir::plaintextType);
  return *this;
}

Plaintext::Plaintext(const std::vector<int64_t> &message)
  : label(datatype::pt_label_prefix + std::to_string(Plaintext::plaintext_id++))
{

  /*
  if ((size_t)message.size() > program->get_dimension())
    throw("Number of messages in one vector is larger than the expcted value ");
  */

  program->insert_node_in_dataflow<Plaintext>(*this);
  program->insert_entry_in_constants_table({this->label, {ir::ConstantTableEntryType::constant, {label, message}}});
}

Plaintext::Plaintext(const std::vector<double> &message)
  : label(datatype::pt_label_prefix + std::to_string(Plaintext::plaintext_id++))
{
  program->insert_node_in_dataflow<Plaintext>(*this);
  program->insert_entry_in_constants_table({this->label, {ir::ConstantTableEntryType::constant, {label, message}}});
}

Plaintext::Plaintext(const std::string &tag, VarType var_type)
  : label(datatype::pt_label_prefix + std::to_string(Plaintext::plaintext_id++))
{
  if (var_type == VarType::input)
  {
    program->insert_node_in_dataflow(*this);
  }
  // we are expecting from the user to provide a tag for input
  operate_in_constants_table(this->label, tag, var_type);
}

Plaintext &Plaintext::operator=(const Plaintext &pt_copy)
{
  return operate_copy_assignement<Plaintext>(*this, pt_copy, ir::plaintextType);
}

Plaintext::Plaintext(const Plaintext &pt_copy) : label(datatype::pt_label_prefix + std::to_string(plaintext_id++))
{
  operate_copy<Plaintext>(*this, pt_copy, ir::plaintextType);
  /*
  auto pt_copy_node_ptr = program->insert_node_in_dataflow<Plaintext>(pt_copy);
  program->insert_operation_node_in_dataflow(ir::OpCode::assign, {pt_copy_node_ptr}, this->label, ir::plaintextType);
  */
  // std::cout << this->label << " = " << pt_copy.get_label() << "\n";
}

Plaintext &Plaintext::operator+=(const Plaintext &rhs)
{

  // if (is_compile_time_evaluation_possible<Plaintext>(*this, rhs))
  // {
  compound_operate<Plaintext, Plaintext>(*this, rhs, ir::OpCode::add, ir::plaintextType);
  return *this;
  // }
  // else
  // throw(datatype::eval_not_supported);
}

Plaintext &Plaintext::operator*=(const Plaintext &rhs)
{
  // if (is_compile_time_evaluation_possible<Plaintext>(*this, rhs))
  // {
  compound_operate<Plaintext, Plaintext>(*this, rhs, ir::OpCode::mul, ir::plaintextType);
  return *this;
  // }
  // else
  // throw(datatype::eval_not_supported);
}

Plaintext &Plaintext::operator-=(const Plaintext &rhs)
{
  // if (is_compile_time_evaluation_possible<Plaintext>(*this, rhs))
  // {
  compound_operate<Plaintext, Plaintext>(*this, rhs, ir::OpCode::sub, ir::plaintextType);
  return *this;
  // }
  // else
  // throw(datatype::eval_not_supported);
}

Plaintext Plaintext::operator-()
{
  return operate_unary<Plaintext, Plaintext>(*this, ir::OpCode::negate, ir::plaintextType);
}

Plaintext operator+(const Plaintext &lhs, const Plaintext &rhs)
{
  // if (is_compile_time_evaluation_possible<Plaintext>(lhs, rhs))
  // {
  return operate_binary<Plaintext, Plaintext, Plaintext>(lhs, rhs, ir::OpCode::add, ir::plaintextType);
  // }
  // else
  // throw(datatype::eval_not_supported);
}

Plaintext operator*(const Plaintext &lhs, const Plaintext &rhs)
{
  // if (is_compile_time_evaluation_possible<Plaintext>(lhs, rhs))
  // {
  return operate_binary<Plaintext, Plaintext, Plaintext>(lhs, rhs, ir::OpCode::mul, ir::plaintextType);
  // }
  // else
  // throw(datatype::eval_not_supported);
}

Plaintext operator-(const Plaintext &lhs, const Plaintext &rhs)
{
  // if (is_compile_time_evaluation_possible<Plaintext>(lhs, rhs))
  // {
  return operate_binary<Plaintext, Plaintext, Plaintext>(lhs, rhs, ir::OpCode::sub, ir::plaintextType);
  // }
  // else
  // throw(datatype::eval_not_supported);
}

Plaintext operator-(Plaintext &rhs)
{
  return operate_unary<Plaintext, Plaintext>(rhs, ir::OpCode::negate, ir::plaintextType);
}

} // namespace fhecompiler
