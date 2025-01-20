#include "fheco/dsl/ciphertext.hpp"
#include "fheco/dsl/compiler.hpp"
#include "fheco/dsl/plaintext.hpp"
#include "fheco/ir/func.hpp"
#include "fheco/util/common.hpp"
#ifdef FHECO_LOGGING
#include "fheco/util/expr_printer.hpp"
#include <iostream>
#endif
#include <stack>
#include <iostream>
#include <stdexcept>
#include <type_traits>
#include <unordered_set>
#include <utility>

using namespace std;

namespace fheco::ir
{
Func::Func(
  string name, size_t slot_count, bool delayed_reduct, int plain_modulus_bit_size, bool signedness, bool need_cyclic_rotation,
  bool overflow_warnings)
  : name_{move(name)}, slot_count_{slot_count} ,plain_modulus_{plain_modulus_bit_size}, need_cyclic_rotation_{need_cyclic_rotation},
    clear_data_eval_{slot_count_, 1<<plain_modulus_bit_size, signedness, delayed_reduct, overflow_warnings}
{
  if (need_cyclic_rotation && !util::is_power_of_two(slot_count_))
    throw invalid_argument("when need_cyclic_rotation, slot_count must be a power of two");
}

template <typename T>
void Func::init_input(T &input, string label)
{
  Term::Type term_type;
  if constexpr (is_same<T, Ciphertext>::value)
    term_type = Term::Type::cipher;
  else
    term_type = Term::Type::plain;
  //std::cout<<"===>insert_input in data_flow :"<<label<<" \n";
  input.id_ = data_flow_.insert_input(term_type, InputTermInfo{move(label), input.example_val_})->id();
}

template <typename T>
void Func::init_const(T &constant, PackedVal packed_val)
{
  clear_data_eval_.adjust_packed_val(packed_val);
  constant.example_val_ = packed_val;
  constant.id_ = insert_const_term(move(packed_val))->id();
}

template <typename TArg, typename TDest>
void Func::operate_unary(OpCode op_code, const TArg &arg, TDest &dest)
{
  auto arg_term = data_flow_.get_term(arg.id());
  if (!arg_term)
    throw invalid_argument("operand not defined");

  dest.shape_ = arg.shape();

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
      clear_data_eval_.operate_unary(op_code, *arg.example_val(), dest_example_val);
      dest.example_val_ = move(dest_example_val);
    }
  }

  vector<Term *> operands{arg_term};
  dest.id_ = insert_op_term(move(op_code), move(operands))->id();
}

template <typename TArg1, typename TArg2, typename TDest>
void Func::operate_binary(OpCode op_code, const TArg1 &arg1, const TArg2 &arg2, TDest &dest)
{
  auto arg1_term = data_flow_.get_term(arg1.id());
  auto arg2_term = data_flow_.get_term(arg2.id());
  if (!arg1_term || !arg2_term)
    throw invalid_argument("operand not defined");

  if (arg1.shape() != arg2.shape())
    throw invalid_argument("operating with incompatible shapes");

  dest.shape_ = arg1.shape();

  if (arg1.example_val() && arg2.example_val())
  {
    PackedVal dest_example_val;
    clear_data_eval_.operate_binary(op_code, *arg1.example_val(), *arg2.example_val(), dest_example_val);
    dest.example_val_ = move(dest_example_val);
  }

  vector<Term *> operands{arg1_term, arg2_term};
  dest.id_ = insert_op_term(move(op_code), move(operands))->id();
}

template <typename T>
void Func::set_output(const T &output, string label)
{
  if (auto term = data_flow_.get_term(output.id()); term)
    data_flow_.set_output(term, OutputTermInfo{unordered_set<string>{move(label)}, output.example_val()});
  else
    throw invalid_argument("object not defined");
}

void Func::update_negative_rotation_steps(int polynomial_modulus_degree){
    data_flow_.update_negative_rotation_steps(polynomial_modulus_degree);
}

Term *Func::insert_op_term(OpCode op_code, vector<Term *> operands, bool &inserted)
{
  if (Compiler::const_folding_enabled())
  {
    vector<PackedVal> operands_vals;
    if (can_fold(operands, operands_vals))
    {
#ifdef FHECO_LOGGING
      util::ExprPrinter expr_printer{Compiler::active_func()};
      clog << "const folding: ";
      if (operands.size() == 1)
        clog << op_code << " " << expr_printer.make_leaf_str_expr(operands[0]);
      else if (operands.size() == 2)
        clog << expr_printer.make_leaf_str_expr(operands[0]) << " " << op_code << " "
             << expr_printer.make_leaf_str_expr(operands[1]);
      clog << '\n';
#endif
      if (op_code.type() == OpCode::Type::encrypt)
        return insert_const_term(operands_vals[0], inserted);

      PackedVal dest_val;
      clear_data_eval_.operate(op_code, operands_vals, dest_val);
      return insert_const_term(dest_val, inserted);
    }
  }
  return data_flow_.insert_op(move(op_code), move(operands), inserted);
}

bool Func::can_fold(const vector<Term *> &operands, vector<PackedVal> &operands_vals) const
{
  operands_vals.reserve(operands.size());
  for (auto operand : operands)
  {
    if (auto val = data_flow_.get_const_val(operand); val)
      operands_vals.push_back(*val);
    else
      return false;
  }
  return true;
}

Term *Func::insert_const_term(PackedVal packed_val, bool &inserted)
{
  return data_flow_.insert_const({util::is_scalar(packed_val), move(packed_val)}, inserted);
}

void Func::replace_term_with(Term *term1, Term *term2)
{
  struct Call
  {
    Term *term1_;
    Term *term2_;
  };
  stack<Call> call_stack;
  call_stack.push(Call{term1, term2});
  while (!call_stack.empty())
  {
    auto top_call = call_stack.top();
    call_stack.pop();
    auto top_term1 = top_call.term1_;
    auto top_term2 = top_call.term2_;

    bool is_const_top_term1 = data_flow_.is_const(top_term1);
    unordered_set<size_t> top_term1_parents_ids;
    for (auto parent : top_term1->parents())
      top_term1_parents_ids.insert(parent->id());
    data_flow_.replace(top_term1, top_term2);
    if (!is_const_top_term1 && Compiler::const_folding_enabled() && data_flow_.is_const(top_term2))
    {
      for (auto parent : top_term2->parents())
      {
        if (top_term1_parents_ids.find(parent->id()) == top_term1_parents_ids.end())
          continue;

        vector<PackedVal> operands_vals;
        if (can_fold(parent->operands(), operands_vals))
        {
          bool inserted;
          if (parent->op_code().type() == OpCode::Type::encrypt)
          {
            call_stack.push(Call{parent, insert_const_term(operands_vals[0], inserted)});
            continue;
          }
          PackedVal dest_val;
          clear_data_eval_.operate(parent->op_code(), operands_vals, dest_val);
          call_stack.push(Call{parent, insert_const_term(dest_val, inserted)});
        }
      }
    }
  }
}

// init_input
template void Func::init_input(Ciphertext &, string);
template void Func::init_input(Plaintext &, string);
// init_const
template void Func::init_const(Ciphertext &, PackedVal);
template void Func::init_const(Plaintext &, PackedVal);
// set_output
template void Func::set_output(const Ciphertext &, string);
template void Func::set_output(const Plaintext &, string);
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
