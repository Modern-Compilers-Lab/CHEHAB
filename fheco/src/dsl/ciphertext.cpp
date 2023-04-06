#include "ciphertext.hpp"
#include "compiler.hpp"
#include "datatypes_const.hpp"
#include "datatypes_util.hpp"
#include "evaluate_on_clear.hpp"
#include <cstdint>
#include <vector>

using namespace std;

namespace fhecompiler
{
size_t Ciphertext::ciphertext_id = 0;

Ciphertext::Ciphertext(const string &tag, long long min_value, long long max_value)
  : label_(datatype::ct_label_prefix + to_string(Ciphertext::ciphertext_id++)), tag_(tag)
{
  Compiler::init_input(label_, tag, min_value, max_value, example_value_);
  Compiler::get_active()->insert_node_in_dataflow(*this);
  operate_in_constants_table(label_, tag, VarType::input);
}

Ciphertext::Ciphertext(const string &tag, const ir::VectorValue &example_value)
  : label_(datatype::ct_label_prefix + to_string(Ciphertext::ciphertext_id++)), tag_(tag)
{
  Compiler::init_input(label_, tag, example_value, example_value_);
  Compiler::get_active()->insert_node_in_dataflow(*this);
  operate_in_constants_table(label_, tag, VarType::input);
}

Ciphertext &Ciphertext::set_output(const string &tag)
{
  // input output
  if (tag_.has_value())
  {
    Ciphertext input = *this;
    set_new_label();
    operate_copy<Ciphertext>(*this, input, ir::TermType::ciphertext);
  }
  tag_ = tag;
  operate_in_constants_table(label_, *tag_, VarType::output);
  Compiler::set_output(label_, *tag_, example_value_);
  return *this;
}

void Ciphertext::set_new_label()
{
  label_ = datatype::ct_label_prefix + std::to_string(Ciphertext::ciphertext_id++);
}

bool Ciphertext::is_output() const
{
  return (Compiler::get_active()->type_of(label_) == ir::ConstantTableEntryType::output);
}
} // namespace fhecompiler
