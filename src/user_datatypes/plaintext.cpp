#include<memory>
#include"program.hpp"
#include"term.hpp"
#include"plaintext.hpp"
#include"datatypes_util.hpp"

extern std::shared_ptr<ir::Program> program;

namespace fhecompiler
{

size_t Plaintext::plaintext_id=0;

using Ptr = std::shared_ptr<ir::Term>;

std::string Plaintext::generate_new_label() { return datatype::ct_label_prefix+std::to_string(Plaintext::plaintext_id++); }

inline void set_new_label(Plaintext& pt)
{
  pt.set_label(datatype::pt_label_prefix+std::to_string(Plaintext::plaintext_id++));
}

void compound_operate(Plaintext& lhs, const Plaintext& rhs, ir::OpCode opcode)
{

  auto lhs_node_ptr = program->insert_node_in_dataflow<Plaintext>(lhs);
  auto rhs_node_ptr = program->insert_node_in_dataflow<Plaintext>(rhs);

  std::string old_label = lhs.get_label();

  set_new_label(lhs);
  auto new_operation_node_ptr = program->insert_operation_node_in_dataflow(opcode, {lhs_node_ptr, rhs_node_ptr}, lhs.get_label(), ir::plaintextType);
  if(lhs_node_ptr->get_output_flag()) 
  {
    lhs_node_ptr->set_output_flag(false);
    new_operation_node_ptr->set_output_flag(true);
  }

  auto table_entry_opt = program->get_entry_form_constants_table(old_label);
  if(table_entry_opt != std::nullopt)
  {
    ir::ConstantTableEntry& table_entry = *table_entry_opt;
    if(table_entry.get_entry_type() == ir::ConstantTableEntry::ConstantTableEntryType::output)
      program->insert_new_entry_from_existing_with_delete(lhs.get_label(), old_label);
  }

}

void Plaintext::set_as_output(const std::string& label) const
{

  auto this_node_ptr = program->find_node_in_dataflow(this->label);
  this_node_ptr->set_output_flag(true);
  auto constant_table_entry = program->get_entry_form_constants_table(this->label);
  if(constant_table_entry == std::nullopt)
  {
    program->insert_entry_in_constants_table({this->label, {ir::ConstantTableEntry::output, (label.length() ? label : this->label+"_"+datatype::output_tag)}});
  }
  else
  {
    ir::ConstantTableEntry& table_entry = *constant_table_entry;
    table_entry.set_entry_type(ir::ConstantTableEntry::output);
  }

}

Plaintext::Plaintext(const std::vector<int64_t>& message): label(datatype::pt_label_prefix+std::to_string(Plaintext::plaintext_id++))
{
  program->insert_node_in_dataflow<Plaintext>(*this);
  program->insert_entry_in_constants_table({this->label, {ir::ConstantTableEntry::constant, {message}}});
}

Plaintext::Plaintext(const std::vector<double>& message): label(datatype::pt_label_prefix+std::to_string(Plaintext::plaintext_id++))
{
  program->insert_node_in_dataflow<Plaintext>(*this);
  program->insert_entry_in_constants_table({this->label, {ir::ConstantTableEntry::constant, {message}}});
}

Plaintext::Plaintext(std::string tag, bool output_flag, bool input_flag): label(datatype::pt_label_prefix+std::to_string(Plaintext::plaintext_id++))
{

  //we are expecting from the user to provide a tag for input

  auto node_ptr = program->insert_node_in_dataflow<Plaintext>(*this);
  node_ptr->set_iutput_flag(input_flag);
  node_ptr->set_output_flag(output_flag);

  if( tag.length() )
  {

    ir::ConstantTableEntry::ConstantTableEntryType entry_type;
    if( input_flag && output_flag ) entry_type = ir::ConstantTableEntry::io;
    else if( input_flag ) entry_type = ir::ConstantTableEntry::input;
    else if( output_flag ) entry_type = ir::ConstantTableEntry::output;
    else entry_type = ir::ConstantTableEntry::constant;

    ir::ConstantTableEntry::EntryValue entry_value = tag;
    program->insert_entry_in_constants_table({this->label, {entry_type, entry_value}});
  }

}

Plaintext& Plaintext::operator=(const Plaintext& pt_copy)
{
  auto this_node_ptr = program->find_node_in_dataflow(this->get_label());

  if( this_node_ptr->get_output_flag() )
  {
    auto pt_copy_node_ptr = program->insert_node_in_dataflow<Plaintext>(pt_copy);
    //inserting new output in data flow as assignement, and in the constatns_table but this time we insert it as a symbol with tag
    std::string old_label = this->label;
    set_new_label(*this);
    program->insert_new_entry_from_existing_with_delete(this->label, old_label);
    auto new_assign_operation = program->insert_operation_node_in_dataflow(ir::assign, {pt_copy_node_ptr}, this->label, ir::plaintextType);
    new_assign_operation->set_output_flag(true);
  }

  else this->label = pt_copy.get_label();

  return *this;
}

Plaintext::Plaintext(const Plaintext& pt_copy): label(datatype::pt_label_prefix + std::to_string(plaintext_id++))
{

  auto pt_copy_node_ptr = program->insert_node_in_dataflow<Plaintext>(pt_copy);
  program->insert_operation_node_in_dataflow(ir::assign, {pt_copy_node_ptr}, this->label, ir::plaintextType);
  //std::cout << this->label << " = " << pt_copy.get_label() << "\n";
}

Plaintext& Plaintext::operator+=(const Plaintext& rhs) 
{

  compound_operate(*this, rhs, ir::add);
  return *this;

}


Plaintext& Plaintext::operator*=(const Plaintext& rhs) 
{
  compound_operate(*this, rhs, ir::mul);
  return *this;
}

Plaintext& Plaintext::operator-=(const Plaintext& rhs) 
{
  compound_operate(*this, rhs, ir::sub);
  return *this;
}

Plaintext Plaintext::operator+(const Plaintext& rhs)
{
  auto lhs_node_ptr = program->insert_node_in_dataflow<Plaintext>(*this);
  auto rhs_node_ptr = program->insert_node_in_dataflow<Plaintext>(rhs);
  return operate<Plaintext>(ir::add, {lhs_node_ptr, rhs_node_ptr}, ir::plaintextType);
}

Plaintext Plaintext::operator-(const Plaintext& rhs)
{ 
  auto lhs_node_ptr = program->insert_node_in_dataflow<Plaintext>(*this);
  auto rhs_node_ptr = program->insert_node_in_dataflow<Plaintext>(rhs);
  return operate<Plaintext>(ir::sub, {lhs_node_ptr, rhs_node_ptr}, ir::plaintextType);
}

Plaintext Plaintext::operator*(const Plaintext& rhs)
{ 
  auto lhs_node_ptr = program->insert_node_in_dataflow<Plaintext>(*this);
  auto rhs_node_ptr = program->insert_node_in_dataflow<Plaintext>(rhs);
  return operate<Plaintext>(ir::mul, {lhs_node_ptr, rhs_node_ptr}, ir::plaintextType);
}

Plaintext Plaintext::operator-()
{
  auto rhs_node_ptr = program->find_node_in_dataflow(this->label);
  return operate<Plaintext>(ir::negate, {rhs_node_ptr}, ir::plaintextType);
}

Plaintext operator+(const Plaintext& lhs, const Plaintext& rhs)
{

  auto lhs_node_ptr = program->insert_node_in_dataflow<Plaintext>(lhs);
  auto rhs_node_ptr = program->insert_node_in_dataflow<Plaintext>(rhs);
  return operate<Plaintext>(ir::add, {lhs_node_ptr, rhs_node_ptr}, ir::plaintextType);

}

Plaintext operator*(const Plaintext& lhs, const Plaintext& rhs)
{

  auto lhs_node_ptr = program->insert_node_in_dataflow<Plaintext>(lhs);
  auto rhs_node_ptr = program->insert_node_in_dataflow<Plaintext>(rhs);
  return operate<Plaintext>(ir::mul, {lhs_node_ptr, rhs_node_ptr}, ir::plaintextType);

}

Plaintext operator-(const Plaintext& lhs, const Plaintext& rhs)
{

  auto lhs_node_ptr = program->insert_node_in_dataflow<Plaintext>(lhs);
  auto rhs_node_ptr = program->insert_node_in_dataflow<Plaintext>(rhs);
  return operate<Plaintext>(ir::sub, {lhs_node_ptr, rhs_node_ptr}, ir::plaintextType);

}

Plaintext operator-(Plaintext& rhs) 
{
  auto rhs_node_ptr = program->insert_node_in_dataflow<Plaintext>(rhs);
  return operate<Plaintext>(ir::negate, {rhs_node_ptr}, ir::plaintextType);
}

} //namespace fhecompiler