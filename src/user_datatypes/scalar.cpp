#include"scalar.hpp"
#include<memory>
#include"program.hpp"
#include"term.hpp"
#include"datatypes_util.hpp"

using namespace datatype;

extern ir::Program* program;

namespace fhecompiler
{

size_t Scalar::scalar_id=0;

using Ptr = std::shared_ptr<ir::Term>;

void Scalar::set_new_label()
{
  this->set_label(datatype::sc_label_prefix+std::to_string(Scalar::scalar_id++));
}

Scalar::Scalar(int64_t _data): data(_data), label(datatype::sc_label_prefix+std::to_string(Scalar::scalar_id++))
{
  program->insert_node_in_dataflow<Scalar>(*this);
  program->insert_entry_in_constants_table({this->label, {ir::ConstantTableEntryType::constant, {_data}}});
}

Scalar::Scalar(double _data): data(_data), label(datatype::sc_label_prefix+std::to_string(Scalar::scalar_id++))
{
  program->insert_node_in_dataflow<Scalar>(*this);
  program->insert_entry_in_constants_table({this->label, {ir::ConstantTableEntryType::constant, {_data}}});
}


Scalar::Scalar(): label(datatype::sc_label_prefix+std::to_string(Scalar::scalar_id++)) 
{
  program->insert_node_in_dataflow<Scalar>(*this);
  program->insert_entry_in_constants_table({this->label, {ir::ConstantTableEntryType::constant, {this->data}}});
}


Scalar::Scalar(const std::string&tag, VarType var_type): label(datatype::sc_label_prefix+std::to_string(Scalar::scalar_id++))
{

  //we are expecting from the user to provide a tag for input
  operate_in_constants_table(this->label, tag, var_type);

}

Scalar& Scalar::operator=(const Scalar& sc_copy)
{
  return operate_assignement<Scalar>(*this, sc_copy, ir::scalarType);
}

Scalar::Scalar(const Scalar& sc_copy): label(datatype::sc_label_prefix + std::to_string(scalar_id++))
{

  auto sc_copy_node_ptr = program->insert_node_in_dataflow<Scalar>(sc_copy);
  program->insert_operation_node_in_dataflow(ir::OpCode::assign, {sc_copy_node_ptr}, this->label, ir::scalarType);
  //std::cout << this->label << " = " << sc_copy.get_label() << "\n";
}

Scalar& Scalar::operator+=(const Scalar& rhs) 
{

  compound_operate<Scalar>(*this, rhs, ir::OpCode::add, ir::scalarType);
  return *this;

}


Scalar& Scalar::operator*=(const Scalar& rhs) 
{
  compound_operate<Scalar>(*this, rhs, ir::OpCode::mul, ir::scalarType);
  return *this;
}

Scalar& Scalar::operator-=(const Scalar& rhs) 
{
  compound_operate(*this, rhs, ir::OpCode::sub, ir::scalarType);
  return *this;
}


Scalar Scalar::operator+(const Scalar& rhs)
{
  return operate_binary<Scalar, Scalar, Scalar>(*this, rhs, ir::OpCode::add, ir::scalarType);
}


Scalar Scalar::operator-(const Scalar& rhs)
{
  return operate_binary<Scalar, Scalar, Scalar>(*this, rhs, ir::OpCode::sub, ir::scalarType);
}


Scalar Scalar::operator*(const Scalar& rhs)
{
  return operate_binary<Scalar, Scalar, Scalar>(*this, rhs, ir::OpCode::mul, ir::scalarType);
}

Scalar Scalar::operator-()
{
  return operate_unary<Scalar>(*this, ir::OpCode::negate, ir::scalarType);
}

Scalar operator+(const Scalar& lhs, const Scalar& rhs)
{
  return operate_binary<Scalar, Scalar, Scalar>(lhs, rhs, ir::OpCode::add, ir::scalarType);
}

Scalar operator*(const Scalar& lhs, const Scalar& rhs)
{
  return operate_binary<Scalar, Scalar, Scalar>(lhs, rhs, ir::OpCode::mul, ir::scalarType);
}

Scalar operator-(const Scalar& lhs, const Scalar& rhs)
{

  return operate_binary<Scalar, Scalar, Scalar>(lhs, rhs, ir::OpCode::sub, ir::scalarType);
}

Scalar operator-(const Scalar& rhs) 
{
  return operate_unary<Scalar>(rhs, ir::OpCode::negate, ir::scalarType);
}

} //namespace fhecompiler
