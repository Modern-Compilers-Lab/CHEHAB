#include "ciphertext.hpp"
#include "compiler.hpp"
#include "datatypes_util.hpp"
#include "term.hpp"
#include <memory>

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
  return operate_assignement<Ciphertext>(*this, ct_move, ir::TermType::ciphertext);
  /*
  operate_move<Ciphertext>(*this, std::move(ct_move), ir::TermType::ciphertext);
  return *this;
  */
}

Ciphertext::Ciphertext(Ciphertext &&ct_move)
{
  operate_move<Ciphertext>(*this, std::move(ct_move), ir::TermType::ciphertext);
}

Ciphertext Ciphertext::encrypt(const Plaintext &pt)
{
  auto pt_ptr = Compiler::get_active()->find_node_in_dataflow(pt.get_label());
  if (pt_ptr == nullptr)
  {
    throw("plaintext to encrypt is not defined, maybe it is only declared.\n");
  }
  return operate_unary<Ciphertext, Plaintext>(pt, ir::OpCode::encrypt, ir::TermType::ciphertext);
}

Ciphertext::Ciphertext(const std::string &tag, VarType var_type)
  : label(datatype::ct_label_prefix + std::to_string(Ciphertext::ciphertext_id++))
{
  if (var_type == VarType::input)
  {
    Compiler::get_active()->insert_node_in_dataflow(*this);
  }
  operate_in_constants_table(this->label, tag, var_type);
}

Ciphertext &Ciphertext::operator=(const Ciphertext &ct_copy)
{
  return operate_assignement<Ciphertext>(*this, ct_copy, ir::TermType::ciphertext);
}

Ciphertext::Ciphertext(const Ciphertext &ct_copy) : label(datatype::ct_label_prefix + std::to_string(ciphertext_id++))
{
  /*
  auto ct_copy_node_ptr = Compiler::get_active()->insert_node_in_dataflow<Ciphertext>(ct_copy);
  Compiler::get_active()->insert_operation_node_in_dataflow(ir::OpCode::assign, {ct_copy_node_ptr}, this->label,
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

// Ciphertext &Ciphertext::exponentiate(uint64_t exponent)
// {
//   compound_operate_with_raw<Ciphertext>(
//     *this, std::to_string(exponent), ir::OpCode::exponentiate, ir::TermType::ciphertext);
//   return *this;
// }

Ciphertext Ciphertext::operator-()
{
  return operate_unary<Ciphertext, Ciphertext>(*this, ir::OpCode::negate, ir::TermType::ciphertext);
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
