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

Ciphertext rotate(const Ciphertext &lhs, int steps);

void Ciphertext::set_new_label()
{
  this->set_label(datatype::ct_label_prefix + std::to_string(Ciphertext::ciphertext_id++));
}

Ciphertext &Ciphertext::operator=(Ciphertext &&ct_move)
{
  return operate_assignement<Ciphertext>(*this, ct_move, ir::ciphertextType);
  /*
  operate_move<Ciphertext>(*this, std::move(ct_move), ir::ciphertextType);
  return *this;
  */
}

Ciphertext::Ciphertext(Ciphertext &&ct_move)
{
  operate_move<Ciphertext>(*this, std::move(ct_move), ir::ciphertextType);
}

Ciphertext Ciphertext::encrypt(const Plaintext &pt)
{
  auto pt_ptr = program->find_node_in_dataflow(pt.get_label());
  if (pt_ptr == nullptr)
  {
    throw("plaintext to encrypt is not defined, maybe it is only declared.\n");
  }
  return operate_unary<Ciphertext, Plaintext>(pt, ir::OpCode::encrypt, ir::ciphertextType);
}

Ciphertext::Ciphertext(const std::string &tag, VarType var_type)
  : label(datatype::ct_label_prefix + std::to_string(Ciphertext::ciphertext_id++))
{
  if (var_type == VarType::input)
  {
    program->insert_node_in_dataflow(*this);
  }
  operate_in_constants_table(this->label, tag, var_type);
}

Ciphertext &Ciphertext::operator=(const Ciphertext &ct_copy)
{
  return operate_assignement<Ciphertext>(*this, ct_copy, ir::ciphertextType);
}

Ciphertext::Ciphertext(const Ciphertext &ct_copy) : label(datatype::ct_label_prefix + std::to_string(ciphertext_id++))
{
  /*
  auto ct_copy_node_ptr = program->insert_node_in_dataflow<Ciphertext>(ct_copy);
  program->insert_operation_node_in_dataflow(ir::OpCode::assign, {ct_copy_node_ptr}, this->label, ir::ciphertextType);
  */
  operate_copy<Ciphertext>(*this, ct_copy, ir::ciphertextType);
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

Ciphertext &Ciphertext::square()
{
  compound_operate_unary<Ciphertext, Ciphertext>(*this, ir::OpCode::square, ir::ciphertextType);
  return *this;
}

Ciphertext &Ciphertext::exponentiate(uint64_t exponent)
{
  compound_operate_with_raw<Ciphertext>(*this, std::to_string(exponent), ir::OpCode::exponentiate, ir::ciphertextType);
  return *this;
}

Ciphertext &Ciphertext::rotate_rows(int steps)
{

  ir::OpCode opcode;

  if (program->get_targeted_backend() != fhecompiler::Backend::SEAL)
  {
    opcode = ir::OpCode::rotate;
  }
  else
    opcode = ir::OpCode::rotate_rows;

  compound_operate_with_raw<Ciphertext>(*this, std::to_string(steps), opcode, ir::ciphertextType);

  return *this;
}

Ciphertext &Ciphertext::rotate(int steps)
{

  ir::OpCode opcode;

  if (program->get_targeted_backend() == fhecompiler::Backend::SEAL)
  {
    opcode = ir::OpCode::rotate_rows;
  }
  else
    opcode = ir::OpCode::rotate;

  compound_operate_with_raw<Ciphertext>(*this, std::to_string(steps), opcode, ir::ciphertextType);

  return *this;
}

Ciphertext &Ciphertext::rotate_columns()
{
  if (program->get_targeted_backend() != Backend::SEAL)
    return *this;
  else
  {
    compound_operate_unary<Ciphertext, Ciphertext>(*this, ir::OpCode::rotate_columns, ir::ciphertextType);
    return *this;
  }
}

Ciphertext Ciphertext::operator-()
{
  return operate_unary<Ciphertext, Ciphertext>(*this, ir::OpCode::negate, ir::ciphertextType);
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
