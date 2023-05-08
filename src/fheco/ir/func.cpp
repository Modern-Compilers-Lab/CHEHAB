#include "fheco/ir/func.hpp"
#include "fheco/dsl/ciphertext.hpp"
#include "fheco/dsl/compiler.hpp"
#include "fheco/dsl/plaintext.hpp"
#include "fheco/util/common.hpp"
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

using namespace std;

namespace fheco::ir
{
Func::Func(string name, size_t slot_count, integer modulus, bool signedness, bool delayed_reduction)
  : name_{move(name)}, slot_count_{slot_count}, need_cyclic_rotations_{false},
    clear_data_evaluator_{slot_count_, modulus, signedness, delayed_reduction}
{
  if (!is_valid_slot_count(slot_count_))
    throw invalid_argument("slot_count must be a power of two");
}

Func::Func(string name, size_t slot_count, int bit_width, bool signedness)
  : Func(move(name), slot_count, (2 << (bit_width - 1)) - 1, signedness, true)
{}

bool Func::is_valid_slot_count(size_t slot_count)
{
  return slot_count != 0 && (slot_count & (slot_count - 1)) == 0;
}

template <typename T>
void Func::init_input(T &input, string label)
{
  input.id_ = data_flow_.insert_leaf(input.term_type())->id();
  inputs_info_.emplace(input.id(), ParamTermInfo{move(label), nullopt});
}

template <typename T>
void Func::init_input(T &input, string label, PackedVal example_val)
{
  input.id_ = data_flow_.insert_leaf(input.term_type())->id();
  clear_data_evaluator_.adjust_packed_val(example_val);
  input.example_val_ = example_val;
  inputs_info_.emplace(input.id_, ParamTermInfo{move(label), move(example_val)});
}

template <typename T>
void Func::init_input(T &input, string label, integer example_val_slot_min, integer example_val_slot_max)
{
  auto example_val = clear_data_evaluator_.make_rand_packed_val(example_val_slot_min, example_val_slot_max);
  init_input(input, move(label), move(example_val));
}

template <typename T>
void Func::init_const(T &constant, PackedVal packed_val)
{
  clear_data_evaluator_.adjust_packed_val(packed_val);
  constant.example_val_ = packed_val;
  if (Compiler::cse_enabled())
  {
    auto it = values_to_constants_.find(packed_val);
    if (it != values_to_constants_.end())
    {
      constant.id_ = it->second;
      return;
    }
  }
  constant.id_ = data_flow_.insert_leaf(constant.term_type())->id();
  constants_values_.emplace(constant.id(), packed_val);
  values_to_constants_.emplace(move(packed_val), constant.id());
}

template <typename TArg, typename TDest>
void Func::operate_unary(OpCode op_code, const TArg &arg, TDest &dest)
{
  auto arg_term = data_flow_.find_term(arg.id());
  if (!arg_term)
    throw invalid_argument("operand not defined");

  if (arg.example_val())
  {
    if constexpr (is_same<TArg, Plaintext>::value && is_same<TDest, Ciphertext>::value)
    {
      if (op_code.type() != OpCode::Type::encrypt)
        throw logic_error("expected encrypt plaintext");

      dest.example_val_ = *arg.example_val();
    }
    else
    {
      PackedVal dest_example_val;
      clear_data_evaluator_.operate_unary(op_code, *arg.example_val(), dest_example_val);
      dest.example_val_ = move(dest_example_val);
    }
  }

  dest.shape_ = arg.shape_;

  vector<Term *> operands{arg_term};
  if (Compiler::cse_enabled())
  {
    if (auto parent = data_flow_.find_op_term(op_code, operands); parent)
    {
      dest.id_ = parent->id();
      return;
    }
  }
  dest.id_ = data_flow_.insert_op_term(move(op_code), move(operands))->id();
}

template <typename TArg1, typename TArg2, typename TDest>
void Func::operate_binary(OpCode op_code, const TArg1 &arg1, const TArg2 &arg2, TDest &dest)
{
  auto arg1_term = data_flow_.find_term(arg1.id());
  auto arg2_term = data_flow_.find_term(arg2.id());
  if (!arg1_term || !arg2_term)
    throw invalid_argument("operand not defined");

  if (arg1.example_val() && arg2.example_val())
  {
    PackedVal dest_example_val;
    clear_data_evaluator_.operate_binary(op_code, *arg1.example_val(), *arg2.example_val(), dest_example_val);
    dest.example_val_ = move(dest_example_val);
  }

  if (arg1.shape_ != arg2.shape_)
    throw invalid_argument("operating with incompatible shapes");

  dest.shape_ = arg1.shape_;

  vector<Term *> operands{arg1_term, arg2_term};
  if (Compiler::cse_enabled())
  {
    if (op_code.commutativity())
    {
      if (*operands[0] > *operands[1])
        swap(operands[0], operands[1]);
    }

    if (auto parent = data_flow_.find_op_term(op_code, operands); parent)
    {
      dest.id_ = parent->id();
      return;
    }
  }
  dest.id_ = data_flow_.insert_op_term(move(op_code), move(operands))->id();
}

const Term *Func::find_term(size_t id) const
{
  if (auto term = data_flow_.find_term(id); term)
    return term;

  throw invalid_argument("term with id not found");
}

template <typename T>
void Func::set_output(const T &output, string label)
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
void Func::set_output(const T &output, string label, PackedVal example_val)
{
  if (auto term = data_flow_.find_term(output.id()); term)
  {
    data_flow_.set_output(term);
    outputs_info_.emplace(output.id(), ParamTermInfo{move(label), move(example_val)});
  }
  else
    throw invalid_argument("output not defined");
}

const ParamTermInfo *Func::get_input_info(size_t id) const
{
  if (auto it = inputs_info_.find(id); it != inputs_info_.end())
    return &it->second;

  return nullptr;
}

const PackedVal *Func::get_const_val(size_t id) const
{
  if (auto it = constants_values_.find(id); it != constants_values_.end())
    return &it->second;

  return nullptr;
}

const ParamTermInfo *Func::get_output_info(size_t id) const
{
  if (auto it = outputs_info_.find(id); it != outputs_info_.end())
    return &it->second;

  return nullptr;
}

TermQualif Func::get_term_qualif(size_t id) const
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
template void Func::init_input(Ciphertext &, string);
template void Func::init_input(Plaintext &, string);
template void Func::init_input(Ciphertext &, string, PackedVal);
template void Func::init_input(Plaintext &, string, PackedVal);
template void Func::init_input(Ciphertext &, string, integer, integer);
template void Func::init_input(Plaintext &, string, integer, integer);
// init_const
template void Func::init_const(Ciphertext &, PackedVal);
template void Func::init_const(Plaintext &, PackedVal);
// set_output
template void Func::set_output(const Ciphertext &, string);
template void Func::set_output(const Plaintext &, string);
template void Func::set_output(const Ciphertext &, string, PackedVal);
template void Func::set_output(const Plaintext &, string, PackedVal);
// operate_unary
template void Func::operate_unary(OpCode, const Ciphertext &, Ciphertext &);
template void Func::operate_unary(OpCode, const Plaintext &, Ciphertext &);
template void Func::operate_unary(OpCode, const Plaintext &, Plaintext &);
// operate_binary
template void Func::operate_binary(OpCode, const Ciphertext &, const Ciphertext &, Ciphertext &);
template void Func::operate_binary(OpCode, const Ciphertext &, const Plaintext &, Ciphertext &);
template void Func::operate_binary(OpCode, const Plaintext &, const Ciphertext &, Ciphertext &);
template void Func::operate_binary(OpCode, const Plaintext &, const Plaintext &, Plaintext &);
} // namespace fheco::ir
