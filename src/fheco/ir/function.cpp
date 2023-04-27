#include "fheco/ir/function.hpp"
#include "fheco/dsl/ciphertext.hpp"
#include "fheco/dsl/compiler.hpp"
#include "fheco/dsl/plaintext.hpp"
#include "fheco/dsl/scalar.hpp"
#include "fheco/util/common.hpp"
#include <algorithm>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>

using namespace std;

namespace fheco::ir

{
size_t Function::compute_slot_count(const vector<size_t> &shape)
{
  size_t slot_count = 1;
  for (auto it = shape.cbegin(); it != shape.cend(); ++it)
    slot_count = util::mul_safe(slot_count, *it);

  return slot_count;
}

Function::Function(string name, size_t slot_count, integer modulus, bool signedness, bool delayed_reduction)
  : name_{move(name)}, shape_{slot_count}, need_cyclic_rotations_{false},
    clear_data_evaluator_{slot_count, modulus, signedness, delayed_reduction}, values_to_constants_{
                                                                                 0, HashConstVal{slot_count}}
{
  if (slot_count == 0 || (slot_count & (slot_count - 1)) != 0)
    throw invalid_argument("slot_count must be a power of two");
}

Function::Function(string name, vector<size_t> shape, integer modulus, bool signedness, bool delayed_reduction)
  : name_{move(name)}, shape_{move(shape)}, need_cyclic_rotations_{false},
    clear_data_evaluator_{compute_slot_count(shape_), modulus, signedness, delayed_reduction},
    values_to_constants_{0, HashConstVal{clear_data_evaluator_.slot_count()}}
{
  size_t slot_count = clear_data_evaluator_.slot_count();
  if (slot_count == 0 || (slot_count & (slot_count - 1)) != 0)
    throw invalid_argument("each shape element must be a power of two");
}

template <typename T>
void Function::init_input(T &input, string label)
{
  input.id_ = data_flow_.insert_leaf(input.term_type())->id();
  input.dim_ = shape_.size();
  inputs_info_.emplace(input.id(), ParamTermInfo{move(label), nullopt});
}

template <typename T>
void Function::init_input(T &input, string label, PackedVal example_val)
{
  input.id_ = data_flow_.insert_leaf(input.term_type())->id();
  input.dim_ = shape_.size();
  clear_data_evaluator_.adjust_packed_val(example_val);
  input.example_val_ = example_val;
  inputs_info_.emplace(input.id_, ParamTermInfo{move(label), move(example_val)});
}

template <typename T>
void Function::init_input(T &input, string label, integer example_val_slot_min, integer example_val_slot_max)
{
  auto example_val = clear_data_evaluator_.make_rand_packed_val(example_val_slot_min, example_val_slot_max);
  init_input(input, move(label), move(example_val));
}

template <typename T, typename TVal>
void Function::init_const(T &constant, TVal val)
{
  if constexpr (is_same<T, Ciphertext>::value || is_same<T, Plaintext>::value)
  {
    constant.dim_ = shape_.size();
    clear_data_evaluator_.adjust_packed_val(val);
  }
  else if constexpr (is_same<T, Scalar>::value)
    clear_data_evaluator_.adjust_scalar_val(val);
  else
    static_assert(always_false_v<T>, "invalid template instantiation");

  constant.example_val_ = val;
  ConstVal const_var = move(val);
  if (Compiler::cse_enabled())
  {
    auto it = values_to_constants_.find(const_var);
    if (it != values_to_constants_.end())
    {
      constant.id_ = it->second;
      return;
    }
  }
  constant.id_ = data_flow_.insert_leaf(constant.term_type())->id();
  constants_values_.emplace(constant.id(), const_var);
  values_to_constants_.emplace(move(const_var), constant.id());
}

template <typename TArg, typename TDest>
void Function::operate_unary(OpCode op_code, const TArg &arg, TDest &dest)
{
  auto arg_term = data_flow_.find_term(arg.id());
  if (!arg_term)
    throw invalid_argument("operand not defined");

  if (arg.example_val())
  {
    if constexpr (is_same<TArg, Scalar>::value && is_same<TDest, Ciphertext>::value)
    {
      if (op_code.type() != OpCode::Type::encrypt)
        throw logic_error("expected encrypt Scalar");

      dest.example_val_ = clear_data_evaluator_.make_packed_val(*arg.example_val());
    }
    else if constexpr (is_same<TDest, Ciphertext>::value || is_same<TDest, Plaintext>::value)
    {
      PackedVal dest_example_val;
      clear_data_evaluator_.operate_unary(op_code, *arg.example_val(), dest_example_val);
      dest.example_val_ = move(dest_example_val);
    }
    else if constexpr (is_same<TDest, Scalar>::value)
    {
      ScalarVal dest_example_val;
      clear_data_evaluator_.operate_unary(op_code, *arg.example_val(), dest_example_val);
      dest.example_val_ = move(dest_example_val);
    }
    else
      static_assert(always_false_v<TDest>, "invalid template instantiation");
  }

  if constexpr (is_same<TArg, Scalar>::value && is_same<TDest, Ciphertext>::value)
    dest.dim_ = shape_.size();
  else if constexpr (is_same<TDest, Ciphertext>::value || is_same<TDest, Plaintext>::value)
    dest.dim_ = arg.dim_;
  else
    static_assert(is_same<TDest, Scalar>::value, "invalid template instantiation");

  vector<Term *> operands{arg_term};

  if (Compiler::cse_enabled())
  {
    auto parent = data_flow_.find_term(op_code, operands);
    if (parent)
    {
      dest.id_ = parent->id();
      return;
    }
  }
  dest.id_ = data_flow_.insert_operation_term(move(op_code), move(operands))->id();
}

template <typename TArg1, typename TArg2, typename TDest>
void Function::operate_binary(OpCode op_code, const TArg1 &arg1, const TArg2 &arg2, TDest &dest)
{
  auto arg1_term = data_flow_.find_term(arg1.id());
  auto arg2_term = data_flow_.find_term(arg2.id());
  if (!arg1_term || !arg2_term)
    throw invalid_argument("operand not defined");

  if (arg1.example_val() && arg2.example_val())
  {
    if constexpr (is_same<TDest, Ciphertext>::value || is_same<TDest, Plaintext>::value)
    {
      PackedVal dest_example_val;
      clear_data_evaluator_.operate_binary(op_code, *arg1.example_val(), *arg2.example_val(), dest_example_val);
      dest.example_val_ = move(dest_example_val);
    }
    else if constexpr (is_same<TDest, Scalar>::value)
    {
      ScalarVal dest_example_val;
      clear_data_evaluator_.operate_binary(op_code, *arg1.example_val(), *arg2.example_val(), dest_example_val);
      dest.example_val_ = move(dest_example_val);
    }
    else
      static_assert(always_false_v<TDest>, "invalid template instantiation");
  }

  if constexpr (is_same<TArg1, Ciphertext>::value || is_same<TArg1, Plaintext>::value)
  {
    if constexpr (is_same<TArg2, Ciphertext>::value || is_same<TArg2, Plaintext>::value)
    {
      if (arg1.dim_ != arg2.dim_)
        throw invalid_argument("operating with incompatible dimensions");
    }
    dest.dim_ = arg1.dim_;
  }
  else if constexpr (is_same<TArg2, Ciphertext>::value || is_same<TArg2, Plaintext>::value)
    dest.dim_ = arg2.dim_;
  else
    static_assert(is_same<TDest, Scalar>::value, "invalid template instantiation");

  vector<Term *> operands{arg1_term, arg2_term};
  if (op_code.commutativity())
    sort(operands.begin(), operands.end(), DAG::CompareTermPtr{});

  if (Compiler::cse_enabled())
  {
    auto parent = data_flow_.find_term(op_code, operands);
    if (parent)
    {
      dest.id_ = parent->id();
      return;
    }
  }
  dest.id_ = data_flow_.insert_operation_term(move(op_code), move(operands))->id();
}

template <typename T>
void Function::set_output(const T &output, string label)
{
  if (auto term = data_flow_.find_term(output.id()); term)
  {
    data_flow_.set_output(term);
    outputs_info_.emplace(output.id(), ParamTermInfo{move(label), nullopt});
  }
  else
    throw invalid_argument("output not defined");
}

template <typename T>
void Function::set_output(const T &output, string label, PackedVal example_val)
{
  if (auto term = data_flow_.find_term(output.id()); term)
  {
    data_flow_.set_output(term);
    outputs_info_.emplace(output.id(), ParamTermInfo{move(label), move(example_val)});
  }
  else
    throw invalid_argument("output not defined");
}

TermQualif Function::get_term_qualif(size_t id) const
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

// init_input
template void Function::init_input(Ciphertext &, string);
template void Function::init_input(Plaintext &, string);
template void Function::init_input(Ciphertext &, string, PackedVal);
template void Function::init_input(Plaintext &, string, PackedVal);
template void Function::init_input(Ciphertext &, string, integer, integer);
template void Function::init_input(Plaintext &, string, integer, integer);
// init_const
template void Function::init_const(Ciphertext &, PackedVal);
template void Function::init_const(Plaintext &, PackedVal);
template void Function::init_const(Scalar &, ScalarVal);
// set_output
template void Function::set_output(const Ciphertext &, string);
template void Function::set_output(const Plaintext &, string);
template void Function::set_output(const Ciphertext &, string, PackedVal);
template void Function::set_output(const Plaintext &, string, PackedVal);
// operate_unary
template void Function::operate_unary(OpCode, const Ciphertext &, Ciphertext &);
template void Function::operate_unary(OpCode, const Plaintext &, Ciphertext &);
template void Function::operate_unary(OpCode, const Plaintext &, Plaintext &);
template void Function::operate_unary(OpCode, const Scalar &, Ciphertext &);
template void Function::operate_unary(OpCode, const Scalar &, Scalar &);
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
} // namespace fheco::ir
