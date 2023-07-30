#pragma once

#include "fheco/ir/common.hpp"
#include <cstddef>
#include <memory>
#include <ostream>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace fheco::ir
{
class Func;
} // namespace fheco::ir

namespace fheco::code_gen
{
struct CtxtObjectInfo
{
  std::size_t id_;
  std::size_t dep_count_;
};

using TermsCtxtObjectsInfo = std::unordered_map<std::size_t, CtxtObjectInfo>;

void gen_func(
  const std::shared_ptr<ir::Func> &func, const std::unordered_set<int> &rotataion_steps, std::ostream &header_os,
  std::string_view header_name, std::ostream &source_os);

void gen_func_decl(const std::string &func_name, std::ostream &os);

void gen_rotation_steps_getter_decl(const std::string &func_name, std::ostream &os);

void gen_func_def_signature(const std::string &func_name, std::ostream &os);

void gen_input_terms(
  const ir::InputTermsInfo &input_terms_info, std::ostream &os, TermsCtxtObjectsInfo &terms_ctxt_objects_info);

void gen_cipher_var_id(std::size_t term_id, std::ostream &os);

void gen_plain_var_id(std::size_t term_id, std::ostream &os);

void gen_const_terms(const ir::ConstTermsValues &const_terms_info, bool signedness, std::ostream &os);

void gen_op_terms(
  const std::shared_ptr<ir::Func> &func, std::ostream &os, TermsCtxtObjectsInfo &terms_ctxt_objects_info);

void gen_output_terms(
  const ir::OutputTermsInfo &output_terms_info, std::ostream &os, const TermsCtxtObjectsInfo &terms_ctxt_objects_info);

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
