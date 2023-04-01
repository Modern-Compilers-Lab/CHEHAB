#include "plaintext.hpp"
#include "compiler.hpp"
#include "datatypes_const.hpp"
#include "datatypes_util.hpp"
#include "evaluate_on_clear.hpp"
#include <vector>

using namespace std;

namespace fhecompiler
{

size_t Plaintext::plaintext_id = 0;

Plaintext::Plaintext(const std::string &tag, long long min_value, long long max_value)
  : label_(datatype::pt_label_prefix + std::to_string(Plaintext::plaintext_id++)), tag_(tag)
{
  Compiler::init_input(label_, tag, min_value, max_value, example_value_);
  Compiler::get_active()->insert_node_in_dataflow(*this);
  operate_in_constants_table(label_, tag, VarType::input);
}

Plaintext::Plaintext(const string &tag, const ir::VectorValue &example_value)
  : label_(datatype::pt_label_prefix + std::to_string(Plaintext::plaintext_id++)), tag_(tag)
{
  Compiler::init_input(label_, tag, example_value, example_value_);
  Compiler::get_active()->insert_node_in_dataflow(*this);
  operate_in_constants_table(label_, tag, VarType::input);
}

Plaintext::Plaintext(const std::vector<std::int64_t> &data)
  : label_(datatype::pt_label_prefix + std::to_string(Plaintext::plaintext_id++))
{
  Compiler::init_const(data, example_value_);
  Compiler::get_active()->insert_node_in_dataflow<Plaintext>(*this);
  Compiler::get_active()->insert_entry_in_constants_table(
    {label_, {ir::ConstantTableEntryType::constant, {label_, data}}});
}

Plaintext::Plaintext(const std::vector<std::uint64_t> &data)
  : label_(datatype::pt_label_prefix + std::to_string(Plaintext::plaintext_id++))
{
  Compiler::init_const(data, example_value_);
  Compiler::get_active()->insert_node_in_dataflow<Plaintext>(*this);
  Compiler::get_active()->insert_entry_in_constants_table(
    {label_, {ir::ConstantTableEntryType::constant, {label_, data}}});
}

Plaintext &Plaintext::set_output(const std::string &tag)
{
  // input output
  if (tag_.has_value())
  {
    Plaintext input = *this;
    set_new_label();
    if (tag != *tag_)
      tag_ = tag;
    else
      tag_ = tag + "(copy)";
    operate_copy<Plaintext>(*this, input, ir::TermType::ciphertext);
  }
  else
    tag_ = tag;
  operate_in_constants_table(label_, *tag_, VarType::output);
  Compiler::set_output(label_, *tag_, example_value_);
  return *this;
}

void Plaintext::set_new_label()
{
  label_ = datatype::pt_label_prefix + std::to_string(Plaintext::plaintext_id++);
}
} // namespace fhecompiler
