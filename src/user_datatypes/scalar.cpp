#include"scalar.hpp"
#include<memory>
#include"program.hpp"
#include"term.hpp"
#include"datatypes_util.hpp"

extern std::shared_ptr<ir::Program> program;

namespace fhecompiler
{

size_t Scalar::scalar_id=0;

using Ptr = std::shared_ptr<ir::Term>;

inline void set_new_label(Scalar& sc)
{
  sc.set_label(datatype::sc_label_prefix+std::to_string(Scalar::scalar_id++));
}

std::string Scalar::generate_new_label() { return datatype::sc_label_prefix+std::to_string(Scalar::scalar_id++); }

void compound_operate(Scalar& lhs, const Scalar& rhs, ir::OpCode opcode)
{

  auto lhs_node_ptr = program->insert_node_in_dataflow<Scalar>(lhs);
  auto rhs_node_ptr = program->insert_node_in_dataflow<Scalar>(rhs);

  std::string old_label = lhs.get_label();

  set_new_label(lhs);
  auto new_operation_node_ptr = program->insert_operation_node_in_dataflow(opcode, {lhs_node_ptr, rhs_node_ptr}, lhs.get_label(), ir::scalarType);
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

void Scalar::set_as_output() const
{

  auto this_node_ptr = program->find_node_in_dataflow(this->label);
  this_node_ptr->set_output_flag(true);
  auto constant_table_entry = program->get_entry_form_constants_table(this->label);
  if(constant_table_entry == std::nullopt)
  {
    program->insert_entry_in_constants_table({this->label, {ir::ConstantTableEntry::output, this->label+"_"+datatype::output_tag}});
  }
  else
  {
    ir::ConstantTableEntry& table_entry = *constant_table_entry;
    table_entry.set_entry_type(ir::ConstantTableEntry::output);
  }

}

Scalar::Scalar(int64_t _data): data(_data), label(datatype::sc_label_prefix+std::to_string(Scalar::scalar_id++))
{
  program->insert_node_in_dataflow<Scalar>(*this);
  program->insert_entry_in_constants_table({this->label, {ir::ConstantTableEntry::constant, {_data}}});
}

Scalar::Scalar(double _data): data(_data), label(datatype::sc_label_prefix+std::to_string(Scalar::scalar_id++))
{
  program->insert_node_in_dataflow<Scalar>(*this);
  program->insert_entry_in_constants_table({this->label, {ir::ConstantTableEntry::constant, {_data}}});
}


Scalar::Scalar(): label(datatype::sc_label_prefix+std::to_string(Scalar::scalar_id++)) 
{
  program->insert_node_in_dataflow<Scalar>(*this);
  program->insert_entry_in_constants_table({this->label, {ir::ConstantTableEntry::constant, {this->data}}});
}


Scalar::Scalar(std::string tag, bool output_flag, bool input_flag): label(datatype::sc_label_prefix+std::to_string(Scalar::scalar_id++))
{

  //we are expecting from the user to provide a tag for input


  if( tag.length() )
  {
   
    auto node_ptr = program->insert_node_in_dataflow<Scalar>(*this);
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

Scalar& Scalar::operator=(const Scalar& sc_copy)
{
  auto this_node_ptr = program->find_node_in_dataflow(this->get_label());

  if( this_node_ptr->get_output_flag() )
  {
    auto sc_copy_node_ptr = program->insert_node_in_dataflow<Scalar>(sc_copy);
    //inserting new output in data flow as assignement, and in the constatns_table but this time we insert it as a symbol with tag
    std::string old_label = this->label;
    set_new_label(*this);
    program->insert_new_entry_from_existing_with_delete(this->label, old_label);
    auto new_assign_operation = program->insert_operation_node_in_dataflow(ir::assign, {sc_copy_node_ptr}, this->label, ir::scalarType);
    new_assign_operation->set_output_flag(true);
  }
  else this->label = sc_copy.get_label();

  return *this;
}

Scalar::Scalar(const Scalar& sc_copy): label(datatype::sc_label_prefix + std::to_string(scalar_id++))
{

  auto sc_copy_node_ptr = program->insert_node_in_dataflow<Scalar>(sc_copy);
  program->insert_operation_node_in_dataflow(ir::assign, {sc_copy_node_ptr}, this->label, ir::scalarType);
  //std::cout << this->label << " = " << sc_copy.get_label() << "\n";
}

Scalar& Scalar::operator+=(const Scalar& rhs) 
{

  compound_operate(*this, rhs, ir::add);
  return *this;

}


Scalar& Scalar::operator*=(const Scalar& rhs) 
{
  compound_operate(*this, rhs, ir::mul);
  return *this;
}

Scalar& Scalar::operator-=(const Scalar& rhs) 
{
  compound_operate(*this, rhs, ir::sub);
  return *this;
}


Scalar Scalar::operator+(const Scalar& rhs)
{

  auto lhs_node_ptr = program->insert_node_in_dataflow<Scalar>(*this);
  auto rhs_node_ptr = program->insert_node_in_dataflow<Scalar>(rhs);
  return operate<Scalar>(ir::add, {lhs_node_ptr, rhs_node_ptr}, ir::scalarType);

}


Scalar Scalar::operator-(const Scalar& rhs)
{

  auto lhs_node_ptr = program->insert_node_in_dataflow<Scalar>(*this);
  auto rhs_node_ptr = program->insert_node_in_dataflow<Scalar>(rhs);
  return operate<Scalar>(ir::sub, {lhs_node_ptr, rhs_node_ptr}, ir::scalarType);

}


Scalar Scalar::operator*(const Scalar& rhs)
{

  auto lhs_node_ptr = program->insert_node_in_dataflow<Scalar>(*this);
  auto rhs_node_ptr = program->insert_node_in_dataflow<Scalar>(rhs);
  return operate<Scalar>(ir::mul, {lhs_node_ptr, rhs_node_ptr}, ir::scalarType);

}

Scalar Scalar::operator-()
{
  auto rhs_node_ptr = program->find_node_in_dataflow(this->label);
  return operate<Scalar>(ir::negate, {rhs_node_ptr}, ir::scalarType);
}

Scalar operator+(const Scalar& lhs, const Scalar& rhs)
{

  auto lhs_node_ptr = program->insert_node_in_dataflow<Scalar>(lhs);
  auto rhs_node_ptr = program->insert_node_in_dataflow<Scalar>(rhs);
  return operate<Scalar>(ir::add, {lhs_node_ptr, rhs_node_ptr}, ir::scalarType);

}

Scalar operator*(const Scalar& lhs, const Scalar& rhs)
{

  auto lhs_node_ptr = program->insert_node_in_dataflow<Scalar>(lhs);
  auto rhs_node_ptr = program->insert_node_in_dataflow<Scalar>(rhs);
  return operate<Scalar>(ir::mul, {lhs_node_ptr, rhs_node_ptr}, ir::scalarType);

}

Scalar operator-(const Scalar& lhs, const Scalar& rhs)
{

  auto lhs_node_ptr = program->insert_node_in_dataflow<Scalar>(lhs);
  auto rhs_node_ptr = program->insert_node_in_dataflow<Scalar>(rhs);
  return operate<Scalar>(ir::sub, {lhs_node_ptr, rhs_node_ptr}, ir::scalarType);

}

Scalar operator-(const Scalar& rhs) 
{
  auto rhs_node_ptr = program->insert_node_in_dataflow<Scalar>(rhs);
  return operate<Scalar>(ir::negate, {rhs_node_ptr}, ir::scalarType);
}

} //namespace fhecompiler
