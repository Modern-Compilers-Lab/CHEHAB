#include "function.hpp"
#include "ciphertext.hpp"
#include "plaintext.hpp"
#include "scalar.hpp"
#include <iostream>
#include <stdexcept>
#include <utility>
#include <vector>

using namespace std;

namespace fhecompiler
{
namespace ir
{
  Function::Function(string name, Scheme scheme, size_t vector_size, int bit_width, bool signedness)
    : name_{move(name)}, scheme_{scheme}, vector_size_{vector_size}, bit_width_{bit_width}, signedness_{signedness},
      data_flow_{}, inputs_info_{}, constants_values_{}, values_to_constants_{0, ConstantValueHash{vector_size_}},
      outputs_info_{}
  // clear_data_evaluator_(vector_size_, bit_width_)
  {
    if (bit_width < 11 || bit_width > 60)
      throw invalid_argument("bit_width must be in [11, 60]");

    if (vector_size == 0 || (vector_size & (vector_size - 1)) != 0)
      throw invalid_argument("vector_size must be a power of two");
  }

  template <typename T>
  void Function::init_input(T &input, string label)
  {
    input.id_ = data_flow_.insert_leaf(input.term_type())->id();
    inputs_info_.emplace(input.id(), ParamsInfo{move(label), nullopt});
  }

  // template <typename T>
  // void Function::init_input(T &input, string label, const VectorValue &example_value)
  // {
  //   input.id_ = data_flow_.insert_leaf(input.term_type())->id_;
  //   inputs_info_.emplace(input.id_, move(label), {});
  // }

  // template <typename T>
  // void Function::init_input(
  //   T &input, string label, int64_t example_value_slot_min, int64_t example_value_slot_max)
  // {
  //   input.id_ = data_flow_.insert_leaf(input.term_type())->id_;
  //   inputs_info_.emplace(input.id_, move(label), {});
  // }

  template <typename T>
  void Function::init_const(T &constant, const ConstantValue &value)
  {
    if (auto it = values_to_constants_.find(value); it != values_to_constants_.end())
      constant.id_ = it->second;
    else
    {
      constant.id_ = data_flow_.insert_leaf(constant.term_type())->id();
      constants_values_.emplace(constant.id(), value);
      values_to_constants_.emplace(value, constant.id());
    }
  }

  template <typename TArg1, typename TArg2, typename TDestination>
  void Function::operate_binary(OpCode op_code, const TArg1 &arg1, const TArg2 &arg2, TDestination &destination)
  {
    auto arg1_term = data_flow_.find_term(arg1.id());
    auto arg2_term = data_flow_.find_term(arg2.id());
    if (!arg1_term || !arg2_term)
      throw invalid_argument("operand not defined");

    vector<Term *> operands{arg1_term, arg2_term};
    if (auto parent = data_flow_.find_term(op_code, operands); parent)
    {
      cout << "cse" << endl;
      destination.id_ = parent->id();
    }
    else
      destination.id_ = data_flow_.insert_operation_term(move(op_code), move(operands))->id();
  }

  template <typename TArg, typename TDestination>
  void Function::operate_unary(OpCode op_code, const TArg &arg, TDestination &destination)
  {
    auto arg_term = data_flow_.find_term(arg.id());
    if (!arg_term)
      throw invalid_argument("operand not defined");

    vector<Term *> operands{arg_term};
    if (auto parent = data_flow_.find_term(op_code, operands); parent)
    {
      cout << "cse" << endl;
      destination.id_ = parent->id();
    }
    else
      destination.id_ = data_flow_.insert_operation_term(move(op_code), move(operands))->id();
  }

  template <typename T>
  void Function::set_output(const T &output, std::string label)
  {
    data_flow_.set_output(data_flow_.find_term(output.id()));
    outputs_info_.emplace(output.id(), ParamsInfo{move(label), nullopt});
  }

  TermQualif Function::get_term_qualif(std::size_t id) const
  {
    if (is_input_term(id))
    {
      if (is_output_term(id))
        return TermQualif::in_out;

      return TermQualif::in;
    }
    if (is_const_term(id))
      return TermQualif::const_;

    if (is_output_term(id))
      return TermQualif::out;

    return TermQualif::temp;
  }

  // explicit template instantiation just to improve compile time
  // init_input
  template void Function::init_input(Ciphertext &, string);
  template void Function::init_input(Plaintext &, string);
  // template void Function::init_input(Ciphertext &, string, const VectorValue &);
  // template void Function::init_input(Plaintext &, string, const VectorValue &);
  // template void Function::init_input(Ciphertext &, string, int64_t, int64_t);
  // template void Function::init_input(Plaintext &, string, int64_t, int64_t);
  // init_const
  template void Function::init_const(Ciphertext &, const ConstantValue &);
  template void Function::init_const(Plaintext &, const ConstantValue &);
  template void Function::init_const(Scalar &, const ConstantValue &);
  // set_output
  template void Function::set_output(const Ciphertext &, string);
  template void Function::set_output(const Plaintext &, string);
  // operate_binary
  template void Function::operate_binary(OpCode, const Ciphertext &, const Ciphertext &, Ciphertext &);
  template void Function::operate_binary(OpCode, const Ciphertext &, const Plaintext &, Ciphertext &);
  template void Function::operate_binary(OpCode, const Ciphertext &, const Scalar &, Ciphertext &);
  template void Function::operate_binary(OpCode, const Plaintext &, const Ciphertext &, Ciphertext &);
  template void Function::operate_binary(OpCode, const Plaintext &, const Plaintext &, Plaintext &);
  template void Function::operate_binary(OpCode, const Plaintext &, const Scalar &, Plaintext &);
  template void Function::operate_binary(OpCode, const Scalar &, const Ciphertext &, Ciphertext &);
  template void Function::operate_binary(OpCode, const Scalar &, const Plaintext &, Plaintext &);
  template void Function::operate_binary(OpCode, const Scalar &, const Scalar &, Scalar &);
  // operate_unary
  template void Function::operate_unary(OpCode, const Ciphertext &, Ciphertext &);
  template void Function::operate_unary(OpCode, const Plaintext &, Ciphertext &);
  template void Function::operate_unary(OpCode, const Plaintext &, Plaintext &);
  template void Function::operate_unary(OpCode, const Scalar &, Ciphertext &);
  template void Function::operate_unary(OpCode, const Scalar &, Scalar &);
} // namespace ir
} // namespace fhecompiler
