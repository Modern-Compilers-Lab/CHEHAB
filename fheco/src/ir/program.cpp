#include "program.hpp"
#include "ciphertext.hpp"
#include "plaintext.hpp"
#include "scalar.hpp"
#include <stdexcept>
#include <utility>
#include <vector>

using namespace std;

namespace fhecompiler
{
namespace ir
{
  Program::Program(string name, Scheme scheme, size_t vector_size, int bit_width, bool signedness)
    : name_{move(name)}, data_flow_{make_unique<DAG>()}, tag_table_{}, scheme_{scheme}, vector_size_{vector_size},
      bit_width_{bit_width}, signedness_{signedness}, clear_data_evaluator_(vector_size_, bit_width_)
  {
    if (bit_width < 11 || bit_width > 60)
      throw invalid_argument("bit_width must be in [11, 60]");

    if (vector_size == 0 || (vector_size & (vector_size - 1)) != 0)
      throw invalid_argument("vector_size must be a power of two");
  }

  void Program::init_input_term(string label, TermType type, string tag)
  {
    if (data_flow_->find_term(label) != nullptr)
      throw invalid_argument("term with label already exists");

    data_flow_->insert_leaf(label, move(type));
    TagTableEntry entry{move(tag), TagTableEntryType::input};
    tag_table_.emplace(move(label), move(entry));
  }

  void Program::init_const_term(string label, TermType type, string tag, VectorValue value)
  {
    if (data_flow_->find_term(label) != nullptr)
      throw invalid_argument("term with label already exists");

    data_flow_->insert_leaf(label, move(type));
    TagTableEntry entry{move(tag), move(value)};
    tag_table_.emplace(move(label), move(entry));
  }

  void Program::init_const_term(string label, TermType type, string tag, ScalarValue value)
  {
    if (data_flow_->find_term(label) != nullptr)
      throw invalid_argument("term with label already exists");

    data_flow_->insert_leaf(label, move(type));
    TagTableEntry entry{move(tag), move(value)};
    tag_table_.emplace(move(label), move(entry));
  }

  template <typename TArg1, typename TArg2, typename TDestination>
  void Program::operate_binary(OpCode op_code, const TArg1 &arg1, const TArg2 &arg2, TDestination &destination)
  {
    auto arg1_term = data_flow_->find_term(arg1.label());
    auto arg2_term = data_flow_->find_term(arg2.label());
    if (arg1_term == nullptr || arg2_term == nullptr)
      throw invalid_argument("operand not defined");

    data_flow_->insert_term(destination.label(), move(op_code), vector<Term *>{arg1_term, arg2_term});
  }

  template <typename TArg, typename TDestination>
  void Program::operate_unary(OpCode op_code, const TArg &arg, TDestination &destination)
  {
    auto arg_term = data_flow_->find_term(arg.label());
    if (arg_term == nullptr)
      throw invalid_argument("operand not defined");

    data_flow_->insert_term(destination.label(), move(op_code), vector<Term *>{arg_term});
  }

  void Program::tag_term(string label, string tag)
  {
    if (data_flow_->find_term(label) == nullptr)
      throw invalid_argument("term with label not found");

    tag_table_[move(label)].set_tag(move(tag));
  }

  void Program::set_term_output(string label, string tag)
  {
    auto t = data_flow_->find_term(label);
    if (t == nullptr)
      throw invalid_argument("term with label not found");

    auto &entry = tag_table_[move(label)];
    if (entry.type() != TagTableEntryType::temp)
      throw invalid_argument("cannot set non temporary term as output");

    entry.set_type(TagTableEntryType::output);
    entry.set_tag(move(tag));
    data_flow_->set_output(t);
  }

  std::optional<reference_wrapper<const TagTableEntry>> Program::get_tag_table_entry(const std::string &label) const
  {
    if (auto it = tag_table_.find(label); it != tag_table_.end())
      return it->second;

    return nullopt;
  }

  TagTableEntryType Program::get_term_qualifiers(const string &label) const
  {
    if (auto it = tag_table_.find(label); it != tag_table_.end())
      return it->second.type();

    return TagTableEntryType::temp;
  }

  // explicit template instantiation just to improve compile time
  // operate_binary
  template void Program::operate_binary(OpCode, const Ciphertext &, const Ciphertext &, Ciphertext &);
  template void Program::operate_binary(OpCode, const Ciphertext &, const Plaintext &, Ciphertext &);
  template void Program::operate_binary(OpCode, const Ciphertext &, const Scalar &, Ciphertext &);
  template void Program::operate_binary(OpCode, const Plaintext &, const Ciphertext &, Ciphertext &);
  template void Program::operate_binary(OpCode, const Plaintext &, const Plaintext &, Plaintext &);
  template void Program::operate_binary(OpCode, const Plaintext &, const Scalar &, Plaintext &);
  template void Program::operate_binary(OpCode, const Scalar &, const Ciphertext &, Ciphertext &);
  template void Program::operate_binary(OpCode, const Scalar &, const Plaintext &, Plaintext &);
  template void Program::operate_binary(OpCode, const Scalar &, const Scalar &, Scalar &);
  // operate_unary
  template void Program::operate_unary(OpCode, const Ciphertext &, Ciphertext &);
  template void Program::operate_unary(OpCode, const Plaintext &, Ciphertext &);
  template void Program::operate_unary(OpCode, const Plaintext &, Plaintext &);
  template void Program::operate_unary(OpCode, const Scalar &, Ciphertext &);
  template void Program::operate_unary(OpCode, const Scalar &, Scalar &);
} // namespace ir
} // namespace fhecompiler
