#include "fheco/ir/function.hpp"
#include "fheco/dsl/ciphertext.hpp"
#include "fheco/dsl/plaintext.hpp"
#include "fheco/dsl/scalar.hpp"
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>

using namespace std;

namespace fheco
{
namespace ir
{
  Function::Function(
    string name, Scheme scheme, size_t slot_count, integer modulus, bool signedness, bool delayed_reduction)
    : name_{move(name)}, scheme_{scheme}, slot_count_{slot_count}, modulus_{modulus}, signedness_{signedness},
      data_flow_{}, inputs_info_{}, constants_values_{}, values_to_constants_{0, ConstValHash{slot_count_}},
      outputs_info_{}, clear_data_evaluator_{slot_count_, modulus_, signedness_, delayed_reduction}
  {
    // if (bit_width < 11 || bit_width > 60)
    //   throw invalid_argument("bit_width must be in [11, 60]");

    if (slot_count == 0 || (slot_count & (slot_count - 1)) != 0)
      throw invalid_argument("vector_size must be a power of two");
  }

  Function::Function(string name, Scheme scheme, size_t slot_count, int bit_width, bool signedness)
    : Function(move(name), scheme, slot_count, (2 << (bit_width - 1)) - 1, signedness, true)
  {}

  template <typename T>
  void Function::init_input(T &input, string label)
  {
    input.id_ = data_flow_.insert_leaf(input.term_type())->id();
    inputs_info_.emplace(input.id(), ParamTermInfo{move(label), nullopt});
  }

  template <typename T>
  void Function::init_input(T &input, string label, PackedVal example_val)
  {
    input.id_ = data_flow_.insert_leaf(input.term_type())->id();
    clear_data_evaluator_.adjust_packed_val(example_val);
    input.example_val_ = example_val;
    inputs_info_.emplace(input.id_, ParamTermInfo{move(label), move(example_val)});
  }

  template <typename T>
  void Function::init_input(T &input, string label, integer example_val_slot_min, integer example_val_slot_max)
  {
    input.id_ = data_flow_.insert_leaf(input.term_type())->id();
    auto example_val = clear_data_evaluator_.make_random_packed_val(example_val_slot_min, example_val_slot_max);
    input.example_val_ = example_val;
    inputs_info_.emplace(input.id_, ParamTermInfo{move(label), move(example_val)});
  }

  template <typename T, typename TVal>
  void Function::init_const(T &constant, TVal val)
  {
    if constexpr (is_same<T, Ciphertext>::value || is_same<T, Plaintext>::value)
      clear_data_evaluator_.adjust_packed_val(val);
    else if constexpr (is_same<T, Scalar>::value)
      clear_data_evaluator_.adjust_scalar_val(val);

    constant.example_val_ = val;
    ConstVal const_var = move(val);
    if (auto it = values_to_constants_.find(const_var); it != values_to_constants_.end())
      constant.id_ = it->second;
    else
    {
      constant.id_ = data_flow_.insert_leaf(constant.term_type())->id();
      constants_values_.emplace(constant.id(), const_var);
      values_to_constants_.emplace(move(const_var), constant.id());
    }
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
    }

    vector<Term *> operands{arg_term};
    if (auto parent = data_flow_.find_term(op_code, operands); parent)
      dest.id_ = parent->id();
    else
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
    }

    vector<Term *> operands{arg1_term, arg2_term};
    if (auto parent = data_flow_.find_term(op_code, operands); parent)
      dest.id_ = parent->id();
    else
      dest.id_ = data_flow_.insert_operation_term(move(op_code), move(operands))->id();
  }

  template <typename T>
  void Function::set_output(const T &output, string label)
  {
    data_flow_.set_output(data_flow_.find_term(output.id()));
    outputs_info_.emplace(output.id(), ParamTermInfo{move(label), nullopt});
  }

  template <typename T>
  void Function::set_output(const T &output, string label, PackedVal example_val)
  {
    data_flow_.set_output(data_flow_.find_term(output.id()));
    outputs_info_.emplace(output.id(), ParamTermInfo{move(label), move(example_val)});
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

  // explicit template instantiation just to improve compile time
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
} // namespace ir
} // namespace fheco
