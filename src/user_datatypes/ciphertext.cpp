#include<memory>
#include"program.hpp"
#include"term.hpp"
#include"ciphertext.hpp"
#include"datatypes_util.hpp"

extern ir::Program* program;

using namespace datatype;

namespace fhecompiler
{

size_t Ciphertext::ciphertext_id=0;

using Ptr = std::shared_ptr<ir::Term>;

inline void set_new_label(Ciphertext& ct)
{
  ct.set_label(datatype::ct_label_prefix+std::to_string(Ciphertext::ciphertext_id++));
}

std::string Ciphertext::generate_new_label() { return datatype::ct_label_prefix+std::to_string(Ciphertext::ciphertext_id++); }

Ciphertext::Ciphertext(Plaintext& pt): label(datatype::ct_label_prefix+std::to_string(Ciphertext::ciphertext_id++))
{
  program->insert_node_in_dataflow<Plaintext>(pt);
  program->insert_node_in_dataflow<Ciphertext>(*this);
  program->insert_entry_in_constants_table({this->label, {ir::ConstantTableEntry::constant, {pt.get_label()}}});
}


void Ciphertext::set_as_output(const std::string& tag) const
{

  auto this_node_ptr = program->find_node_in_dataflow(this->label);
  this_node_ptr->set_output_flag(true);
  auto constant_table_entry = program->get_entry_form_constants_table(this->label);

  if(constant_table_entry == std::nullopt)
  {
    std::string tag_to_insert = tag.length() ? tag : (this->label+"_"+datatype::output_tag);
    program->insert_entry_in_constants_table({this->label, {ir::ConstantTableEntry::ConstantTableEntryType::output, {tag_to_insert}}});
  }
  else
  {
    ir::ConstantTableEntry& table_entry = *constant_table_entry;
    table_entry.set_entry_type(ir::ConstantTableEntry::output);
    table_entry.set_entry_tag(tag);
  }

}

Ciphertext::Ciphertext(std::string tag, bool output_flag, bool input_flag): label(datatype::ct_label_prefix+std::to_string(Ciphertext::ciphertext_id++))
{

  //we are expecting from the user to provide a tag for input

  
  if( tag.length() )
  {
    
    auto node_ptr = program->insert_node_in_dataflow<Ciphertext>(*this);
    node_ptr->set_iutput_flag(input_flag);
    node_ptr->set_output_flag(output_flag);

    ir::ConstantTableEntry::ConstantTableEntryType entry_type;
    if( input_flag && output_flag ) entry_type = ir::ConstantTableEntry::io;
    else if( input_flag ) entry_type = ir::ConstantTableEntry::input;
    else if( output_flag ) entry_type = ir::ConstantTableEntry::output;
    else entry_type = ir::ConstantTableEntry::constant;

    ir::ConstantTableEntry::EntryValue entry_value = tag;
    program->insert_entry_in_constants_table({this->label, {entry_type, entry_value}});
  }

}

Ciphertext& Ciphertext::operator=(const Ciphertext& ct_copy)
{

  auto this_node_ptr = program->find_node_in_dataflow(this->get_label());

  if( this_node_ptr->get_output_flag() )
  {
    auto ct_copy_node_ptr = program->insert_node_in_dataflow<Ciphertext>(ct_copy);
    //inserting new output in data flow as assignement, and in the constatns_table but this time we insert it as a symbol with tag
    std::string old_label = this->label;
    set_new_label(*this);
    program->insert_new_entry_from_existing_with_delete(this->label, old_label);
    auto new_assign_operation = program->insert_operation_node_in_dataflow(ir::assign, {ct_copy_node_ptr}, this->label, ir::ciphertextType);
    new_assign_operation->set_output_flag(true);
  }

  else this->label = ct_copy.get_label();

  return *this;
}

Ciphertext::Ciphertext(const Ciphertext& ct_copy): label(datatype::ct_label_prefix + std::to_string(ciphertext_id++))
{
  auto ct_copy_node_ptr = program->insert_node_in_dataflow<Ciphertext>(ct_copy);
  program->insert_operation_node_in_dataflow(ir::assign, {ct_copy_node_ptr}, this->label, ir::ciphertextType);
  //std::cout << this->label << " = " << ct_copy.get_label() << "\n";

}

Ciphertext& Ciphertext::operator+=(const Ciphertext& rhs) 
{
  compound_operate<Ciphertext>(*this, rhs, ir::add, ir::ciphertextType);
  return *this;
}


Ciphertext& Ciphertext::operator*=(const Ciphertext& rhs) 
{
  compound_operate<Ciphertext>(*this, rhs, ir::mul, ir::ciphertextType);
  return *this;
}

Ciphertext& Ciphertext::operator-=(const Ciphertext& rhs) 
{
  compound_operate<Ciphertext>(*this, rhs, ir::sub, ir::ciphertextType);
  return *this;
}


Ciphertext Ciphertext::operator+(const Ciphertext& rhs)
{
  return operate_binary<Ciphertext>(*this, rhs, ir::add, ir::ciphertextType);
}

Ciphertext Ciphertext::operator-(const Ciphertext& rhs)
{
  return operate_binary<Ciphertext>(*this, rhs, ir::sub, ir::ciphertextType);
}

Ciphertext Ciphertext::operator*(const Ciphertext& rhs)
{
  return operate_binary<Ciphertext>(*this, rhs, ir::mul, ir::ciphertextType);
}

Ciphertext Ciphertext::operator-()
{
  return operate_unary<Ciphertext>(*this, ir::negate, ir::ciphertextType);
}

Ciphertext operator+(const Ciphertext& lhs, const Ciphertext& rhs)
{
  return operate_binary<Ciphertext>(lhs, rhs, ir::add, ir::ciphertextType);
}

Ciphertext operator*(const Ciphertext& lhs, const Ciphertext& rhs)
{
  return operate_binary<Ciphertext>(lhs, rhs, ir::mul, ir::ciphertextType);
}

Ciphertext operator-(const Ciphertext& lhs, const Ciphertext& rhs)
{
  return operate_binary<Ciphertext>(lhs, rhs, ir::sub, ir::ciphertextType);
}

Ciphertext operator-(const Ciphertext& rhs)
{
  return operate_unary<Ciphertext>(rhs, ir::negate, ir::ciphertextType);
}

std::string Ciphertext::get_term_tag()
{
  auto table_entry = program->get_entry_form_constants_table(this->label);
  if(table_entry != std::nullopt)
  {
    ir::ConstantTableEntry table_entry_dereferenced = *table_entry;
    return table_entry_dereferenced.get_entry_value().tag;
  }
  else return "";
}

} //namespace fhecompiler