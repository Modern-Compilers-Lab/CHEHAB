#include "ciphertext.hpp"
#include "datatypes_util.hpp"
#include "program.hpp"
#include "term.hpp"
#include <memory>

extern ir::Program *program;

using namespace datatype;

namespace fhecompiler
{

size_t Ciphertext::ciphertext_id = 0;

using Ptr = std::shared_ptr<ir::Term>;

void Ciphertext::set_new_label()
{
  this->set_label(datatype::ct_label_prefix + std::to_string(Ciphertext::ciphertext_id++));
}

Ciphertext Ciphertext::encrypt(const Plaintext &pt)
{
  Ciphertext new_ct("");
  program->insert_node_in_dataflow<Plaintext>(pt);
  program->insert_node_in_dataflow<Ciphertext>(new_ct);
  program->insert_entry_in_constants_table(
      {new_ct.get_label(), {ir::ConstantTableEntryType::constant, {pt.get_label()}}});
  return new_ct;
}

Ciphertext::Ciphertext(const std::string &tag, VarType var_type)
    : label(datatype::ct_label_prefix + std::to_string(Ciphertext::ciphertext_id++))
{
  // we are expecting from the user to provide a tag for input
  // this constructor and function needs to be revised later
  operate_in_constants_table(this->label, tag, var_type);
}

Ciphertext &Ciphertext::operator=(const Ciphertext &ct_copy)
{
  return operate_assignement<Ciphertext>(*this, ct_copy, ir::ciphertextType);
}

Ciphertext::Ciphertext(const Ciphertext &ct_copy) : label(datatype::ct_label_prefix + std::to_string(ciphertext_id++))
{
  auto ct_copy_node_ptr = program->insert_node_in_dataflow<Ciphertext>(ct_copy);
  program->insert_operation_node_in_dataflow(ir::OpCode::assign, {ct_copy_node_ptr}, this->label, ir::ciphertextType);
  // std::cout << this->label << " = " << ct_copy.get_label() << "\n";
}

Ciphertext &Ciphertext::operator+=(const Ciphertext &rhs)
{
  compound_operate<Ciphertext, Ciphertext>(*this, rhs, ir::OpCode::add, ir::ciphertextType);
  return *this;
}

Ciphertext &Ciphertext::operator*=(const Ciphertext &rhs)
{
  compound_operate<Ciphertext, Ciphertext>(*this, rhs, ir::OpCode::mul, ir::ciphertextType);
  return *this;
}

Ciphertext &Ciphertext::operator-=(const Ciphertext &rhs)
{
  compound_operate<Ciphertext, Ciphertext>(*this, rhs, ir::OpCode::sub, ir::ciphertextType);
  return *this;
}

Ciphertext &Ciphertext::square(const Scalar &rhs)
{
  compound_operate<Ciphertext, Scalar>(*this, rhs, ir::OpCode::square, ir::ciphertextType);
  return *this;
}

Ciphertext &Ciphertext::exponentiate(const Scalar &rhs)
{
  compound_operate<Ciphertext, Scalar>(*this, rhs, ir::OpCode::exponentiate, ir::ciphertextType);
  return *this;
}

Ciphertext &Ciphertext::rotate(const Scalar &rhs)
{
  compound_operate<Ciphertext, Scalar>(*this, rhs, ir::OpCode::rotate, ir::ciphertextType);
  return *this;
}

Ciphertext Ciphertext::operator-()
{
  return operate_unary<Ciphertext>(*this, ir::OpCode::negate, ir::ciphertextType);
}

Ciphertext operator+(const Ciphertext &lhs, const Ciphertext &rhs)
{
  return operate_binary<Ciphertext, Ciphertext, Ciphertext>(lhs, rhs, ir::OpCode::add, ir::ciphertextType);
}

Ciphertext operator*(const Ciphertext &lhs, const Ciphertext &rhs)
{
  return operate_binary<Ciphertext, Ciphertext, Ciphertext>(lhs, rhs, ir::OpCode::mul, ir::ciphertextType);
}

Ciphertext operator-(const Ciphertext &lhs, const Ciphertext &rhs)
{
  return operate_binary<Ciphertext, Ciphertext, Ciphertext>(lhs, rhs, ir::OpCode::sub, ir::ciphertextType);
}

Ciphertext operator-(const Ciphertext &rhs)
{
  return operate_unary<Ciphertext>(rhs, ir::OpCode::negate, ir::ciphertextType);
}

Ciphertext exponentiate(const Ciphertext &lhs, const Scalar &rhs)
{
  return operate_binary<Ciphertext, Ciphertext, Scalar>(lhs, rhs, ir::OpCode::exponentiate, ir::ciphertextType);
}

Ciphertext square(const Ciphertext &lhs, const Scalar &rhs)
{
  return operate_binary<Ciphertext, Ciphertext, Scalar>(lhs, rhs, ir::OpCode::square, ir::ciphertextType);
}

Ciphertext rotate(const Ciphertext &lhs, const Scalar &rhs)
{
  return operate_binary<Ciphertext, Ciphertext, Scalar>(lhs, rhs, ir::OpCode::rotate, ir::ciphertextType);
}

std::string Ciphertext::get_term_tag()
{
  auto table_entry = program->get_entry_form_constants_table(this->label);
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
  return (program->type_of(this->label) == ir::ConstantTableEntryType::output);
}

} // namespace fhecompiler