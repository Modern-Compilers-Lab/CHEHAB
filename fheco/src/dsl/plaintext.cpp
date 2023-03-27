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
  operate_move_assignement<Plaintext>(*this, std::move(pt_move), ir::TermType::plaintext);
  return *this;
}

Plaintext::Plaintext(const std::vector<std::int64_t> &message)
  : label(datatype::pt_label_prefix + std::to_string(Plaintext::plaintext_id++))
{

  /*
  if ((size_t)message.size() > program->get_dimension())
    throw("Number of messages in one vector is larger than the expcted value ");
  */

  program->insert_node_in_dataflow<Plaintext>(*this);
  program->insert_entry_in_constants_table({this->label, {ir::ConstantTableEntryType::constant, {label, message}}});
}

Plaintext::Plaintext(const std::vector<std::uint64_t> &message)
  : label(datatype::pt_label_prefix + std::to_string(Plaintext::plaintext_id++))
{

  /*
  if ((size_t)message.size() > program->get_dimension())
    throw("Number of messages in one vector is larger than the expcted value ");
  */

  if (program == nullptr)
    throw(program_not_init_msg);

  program->insert_node_in_dataflow<Plaintext>(*this);
  if (message.size() < program->get_vector_size())
  {
    auto message_resized = message;
    // message_resized.resize(program->get_vector_size());
    program->insert_entry_in_constants_table(
      {this->label, {ir::ConstantTableEntryType::constant, {label, message_resized}}});
  }
  else
  {
    program->insert_entry_in_constants_table({this->label, {ir::ConstantTableEntryType::constant, {label, message}}});
  }
}

Plaintext::Plaintext(const std::vector<double> &message)
  : label(datatype::pt_label_prefix + std::to_string(Plaintext::plaintext_id++))
{

  if (program == nullptr)
    throw(program_not_init_msg);

  program->insert_node_in_dataflow<Plaintext>(*this);
  if (message.size() < program->get_vector_size())
  {
    auto message_resized = message;
    // message_resized.resize(program->get_vector_size());
    program->insert_entry_in_constants_table(
      {this->label, {ir::ConstantTableEntryType::constant, {label, message_resized}}});
  }
  else
  {
    program->insert_entry_in_constants_table({this->label, {ir::ConstantTableEntryType::constant, {label, message}}});
  }
}

Plaintext::Plaintext(const std::string &tag, VarType var_type)
  : label(datatype::pt_label_prefix + std::to_string(Plaintext::plaintext_id++))
{

  if (program == nullptr)
    throw(program_not_init_msg);

  if (var_type == VarType::input)
  {
    program->insert_node_in_dataflow(*this);
  }
  // we are expecting from the user to provide a tag for input
  operate_in_constants_table(this->label, tag, var_type);
}

Plaintext &Plaintext::operator=(const Plaintext &pt_copy)
{
  return operate_copy_assignement<Plaintext>(*this, pt_copy, ir::TermType::plaintext);
}

Plaintext::Plaintext(const Plaintext &pt_copy) : label(datatype::pt_label_prefix + std::to_string(plaintext_id++))
{
  operate_copy<Plaintext>(*this, pt_copy, ir::TermType::plaintext);
  /*
  auto pt_copy_node_ptr = program->insert_node_in_dataflow<Plaintext>(pt_copy);
  program->insert_operation_node_in_dataflow(ir::OpCode::assign, {pt_copy_node_ptr}, this->label,
  ir::TermType::plaintext);
  */
  // std::cout << this->label << " = " << pt_copy.get_label() << "\n";
}

Plaintext &Plaintext::operator+=(const Plaintext &rhs)
{

  // if (is_compile_time_evaluation_possible<Plaintext>(*this, rhs))
  // {
  compound_operate<Plaintext, Plaintext>(*this, rhs, ir::OpCode::add, ir::TermType::plaintext);
  return *this;
  // }
  // else
  // throw(datatype::eval_not_supported);
}

Plaintext &Plaintext::operator*=(const Plaintext &rhs)
{
  // if (is_compile_time_evaluation_possible<Plaintext>(*this, rhs))
  // {
  compound_operate<Plaintext, Plaintext>(*this, rhs, ir::OpCode::mul, ir::TermType::plaintext);
  return *this;
  // }
  // else
  // throw(datatype::eval_not_supported);
}

Plaintext &Plaintext::operator-=(const Plaintext &rhs)
{
  // if (is_compile_time_evaluation_possible<Plaintext>(*this, rhs))
  // {
  compound_operate<Plaintext, Plaintext>(*this, rhs, ir::OpCode::sub, ir::TermType::plaintext);
  return *this;
  // }
  // else
  // throw(datatype::eval_not_supported);
}

Plaintext Plaintext::operator-()
{
  if (is_compile_time_evaluation_possible(*this, *this))
  {
    Plaintext this_copy = *this;
    ir::negate_value_if_possible(this_copy.get_label(), program);
    return this_copy;
  }

  return operate_unary<Plaintext, Plaintext>(*this, ir::OpCode::negate, ir::TermType::plaintext);
}

Plaintext operator+(const Plaintext &lhs, const Plaintext &rhs)
{
  // if (is_compile_time_evaluation_possible<Plaintext>(lhs, rhs))
  // {
  return operate_binary<Plaintext, Plaintext, Plaintext>(lhs, rhs, ir::OpCode::add, ir::TermType::plaintext);
  // }
  // else
  // throw(datatype::eval_not_supported);
}

Plaintext operator*(const Plaintext &lhs, const Plaintext &rhs)
{
  // if (is_compile_time_evaluation_possible<Plaintext>(lhs, rhs))
  // {
  return operate_binary<Plaintext, Plaintext, Plaintext>(lhs, rhs, ir::OpCode::mul, ir::TermType::plaintext);
  // }
  // else
  // throw(datatype::eval_not_supported);
}

Plaintext operator-(const Plaintext &lhs, const Plaintext &rhs)
{
  // if (is_compile_time_evaluation_possible<Plaintext>(lhs, rhs))
  // {
  return operate_binary<Plaintext, Plaintext, Plaintext>(lhs, rhs, ir::OpCode::sub, ir::TermType::plaintext);
  // }
  // else
  // throw(datatype::eval_not_supported);
}

Plaintext operator-(Plaintext &rhs)
{

  if (program == nullptr)
    throw(program_not_init_msg);

  if (is_compile_time_evaluation_possible(rhs, rhs))
  {
    Plaintext copy_rhs = rhs;
    ir::negate_value_if_possible(copy_rhs.get_label(), program);
    return copy_rhs;
  }
  return operate_unary<Plaintext, Plaintext>(rhs, ir::OpCode::negate, ir::TermType::plaintext);
}

} // namespace fhecompiler
