#pragma once

#include "fheco/ir/common.hpp"
#include <cstddef>
#include <memory>
#include <ostream>
#include <string_view>
#include <unordered_set>
#include <vector>

namespace fheco::ir
{
class Func;
class Term;
} // namespace fheco::ir

namespace fheco::code_gen
{
void gen_func(
  const std::shared_ptr<ir::Func> &func, const std::unordered_set<int> &rotataion_steps, std::ostream &header_os,
  std::string_view header_name, std::ostream &source_os);

void gen_func_decl(const std::string &func_name, std::ostream &os);

void gen_rotation_steps_getter_decl(const std::string &func_name, std::ostream &os);

void gen_func_def_signature(const std::string &func_name, std::ostream &os);

void gen_input_terms(const ir::IOTermsInfo &input_terms_info, std::ostream &os);

void gen_cipher_var_id(std::size_t term_id, std::ostream &os);

void gen_plain_var_id(std::size_t term_id, std::ostream &os);

void gen_const_terms(const ir::ConstTermsValues &const_terms_info, bool signedness, std::ostream &os);

void gen_op_terms(const std::vector<const ir::Term *> &top_sorted_terms, std::ostream &os);

void gen_output_terms(const ir::IOTermsInfo &output_terms_info, std::ostream &os);

void gen_rotation_steps_getter_def(
  const std::string &func_name, const std::unordered_set<int> &steps, std::ostream &os);

template <typename Iter>
void gen_sequence(Iter begin, Iter end, std::size_t line_threshold, std::ostream &os)
{
  if (begin == end)
    return;

  size_t i = 0;
  for (Iter it = begin;;)
  {
    os << *it;
    ++it;
    ++i;
    if (it == end)
      break;

    if (i == line_threshold)
    {
      os << ",\n";
      i = 0;
    }
    else
      os << ", ";
  }
}
} // namespace fheco::code_gen
