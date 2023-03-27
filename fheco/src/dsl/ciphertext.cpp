#include "ciphertext.hpp"
#include "datatypes_util.hpp"
#include "program.hpp"
#include "term.hpp"
#include <memory>

using namespace datatype;

extern ir::Program *program;

namespace fhecompiler
{

size_t Ciphertext::ciphertext_id = 0;

using Ptr = std::shared_ptr<ir::Term>;

void Ciphertext::set_new_label()
{
  this->set_label(datatype::ct_label_prefix + std::to_string(Ciphertext::ciphertext_id++));
}

Ciphertext::Ciphertext() : label(datatype::ct_label_prefix + std::to_string(Ciphertext::ciphertext_id++))
{
  operate_in_constants_table(label, label, VarType::temp);
}

Ciphertext &Ciphertext::operator=(Ciphertext &&ct_move)
{
  // return operate_assignement<Ciphertext>(*this, ct_move, ir::TermType::ciphertext);
  operate_move_assignement<Ciphertext>(*this, std::move(ct_move), ir::TermType::ciphertext);
  return *this;
}

Ciphertext::Ciphertext(Ciphertext &&ct_move)
  : label(datatype::ct_label_prefix + std::to_string(Ciphertext::ciphertext_id++))
{
  operate_move<Ciphertext>(*this, std::move(ct_move), ir::TermType::ciphertext);
}

Ciphertext Ciphertext::encrypt(const Plaintext &pt)
{
  if (program == nullptr)
    throw(program_not_init_msg);

  auto pt_ptr = program->find_node_in_dataflow(pt.get_label());
  if (pt_ptr == nullptr)
  {
    throw("plaintext to encrypt is not defined, maybe it is only declared.\n");
  }
  return operate_unary<Ciphertext, Plaintext>(pt, ir::OpCode::encrypt, ir::TermType::ciphertext);
}

Ciphertext::Ciphertext(const std::string &tag, VarType var_type)
  : label(datatype::ct_label_prefix + std::to_string(Ciphertext::ciphertext_id++))
{

  if (program == nullptr)
    throw(program_not_init_msg);

  if (var_type == VarType::input)
  {
    program->insert_node_in_dataflow(*this);
  }
  operate_in_constants_table(this->label, tag, var_type);
}

Ciphertext &Ciphertext::operator=(const Ciphertext &ct_copy)
{
  return operate_copy_assignement<Ciphertext>(*this, ct_copy, ir::TermType::ciphertext);
}

Ciphertext::Ciphertext(const Ciphertext &ct_copy) : label(datatype::ct_label_prefix + std::to_string(ciphertext_id++))
{
  /*
  auto ct_copy_node_ptr = program->insert_node_in_dataflow<Ciphertext>(ct_copy);
  program->insert_operation_node_in_dataflow(ir::OpCode::assign, {ct_copy_node_ptr}, this->label,
  ir::TermType::ciphertext);
  */
  operate_copy<Ciphertext>(*this, ct_copy, ir::TermType::ciphertext);
}

Ciphertext &Ciphertext::operator+=(const Ciphertext &rhs)
{
  compound_operate<Ciphertext, Ciphertext>(*this, rhs, ir::OpCode::add, ir::TermType::ciphertext);
  return *this;
}

Ciphertext &Ciphertext::operator*=(const Ciphertext &rhs)
{
  compound_operate<Ciphertext, Ciphertext>(*this, rhs, ir::OpCode::mul, ir::TermType::ciphertext);
  return *this;
}

Ciphertext &Ciphertext::operator-=(const Ciphertext &rhs)
{
  compound_operate<Ciphertext, Ciphertext>(*this, rhs, ir::OpCode::sub, ir::TermType::ciphertext);
  return *this;
}

Ciphertext &Ciphertext::square()
{
  compound_operate_unary<Ciphertext, Ciphertext>(*this, ir::OpCode::square, ir::TermType::ciphertext);
  return *this;
}

Ciphertext &Ciphertext::exponentiate(uint64_t exponent)
{
  // compound_operate_with_raw<Ciphertext>(*this, std::to_string(exponent), ir::OpCode::exponentiate,
  // ir::TermType::ciphertext); return *this;
  // we convert exponentiate to squares
  if (exponent == 1)
    return *this;

  Ciphertext unit_cipher = *this;
  Ciphertext square_cipher = (*this).square();

  Ciphertext exponentiation = square_cipher;
  exponent -= 2;

  while (exponent > 0)
  {
    if (exponent >= 2)
    {
      exponentiation *= square_cipher;
      exponent -= 2;
    }
    else if (exponent == 1)
    {
      exponentiation *= unit_cipher;
      exponent -= 1;
    }
  }

  *this = exponentiation;

  return *this;
}

Ciphertext &Ciphertext::rotate_rows(int steps)
{

  ir::OpCode opcode = ir::OpCode::rotate;

  /*
  if (program->get_targeted_backend() != fhecompiler::Backend::SEAL)
  {
    opcode = ir::OpCode::rotate;
  }
  else
    opcode = ir::OpCode::rotate_rows;
  */

  compound_operate_with_raw<Ciphertext>(*this, std::to_string(steps), opcode, ir::TermType::ciphertext);

  return *this;
}

Ciphertext &Ciphertext::rotate(int steps)
{

  if (program == nullptr)
    throw(program_not_init_msg);

  ir::OpCode opcode = ir::OpCode::rotate;

  compound_operate_with_raw<Ciphertext>(*this, std::to_string(steps), opcode, ir::TermType::ciphertext);

  return *this;
}

Ciphertext &Ciphertext::rotate_columns()
{

  if (program == nullptr)
    throw(program_not_init_msg);

  if (program->get_targeted_backend() != Backend::SEAL)
    return *this;
  else
  {
    compound_operate_unary<Ciphertext, Ciphertext>(*this, ir::OpCode::rotate_columns, ir::TermType::ciphertext);
    return *this;
  }
}

Ciphertext Ciphertext::operator-()
{
  return operate_unary<Ciphertext, Ciphertext>(*this, ir::OpCode::negate, ir::TermType::ciphertext);
}

Ciphertext operator+(const Ciphertext &lhs, const Ciphertext &rhs)
{
  /*
    Ciphertext lhs_copy = lhs;
    lhs_copy += rhs;
    return lhs_copy;
  */
  return operate_binary<Ciphertext, Ciphertext, Ciphertext>(lhs, rhs, ir::OpCode::add, ir::TermType::ciphertext);
}

Ciphertext operator*(const Ciphertext &lhs, const Ciphertext &rhs)
{
  return operate_binary<Ciphertext, Ciphertext, Ciphertext>(lhs, rhs, ir::OpCode::mul, ir::TermType::ciphertext);
}

Ciphertext operator-(const Ciphertext &lhs, const Ciphertext &rhs)
{
  return operate_binary<Ciphertext, Ciphertext, Ciphertext>(lhs, rhs, ir::OpCode::sub, ir::TermType::ciphertext);
}

Ciphertext operator-(const Ciphertext &rhs)
{
  return operate_unary<Ciphertext, Ciphertext>(rhs, ir::OpCode::negate, ir::TermType::ciphertext);
}

Ciphertext exponentiate(const Ciphertext &lhs, uint32_t exponent)
{
  // return operate_with_raw<Ciphertext>(lhs, std::to_string(exponent), ir::OpCode::exponentiate,
  // ir::TermType::ciphertext);
  Ciphertext lhs_copy = lhs;
  return lhs_copy.exponentiate(exponent);
}

} // namespace fhecompiler
