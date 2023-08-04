#include "fheco/ir/func.hpp"
#include "fheco/ir/term.hpp"
#include "fheco/passes/insert_relin.hpp"
#include <algorithm>
#include <stdexcept>
#include <unordered_map>
#include <utility>

using namespace std;

namespace fheco::passes
{
size_t lazy_relin_heuristic(const shared_ptr<ir::Func> &func, size_t ctxt_size_threshold)
{
  if (ctxt_size_threshold < 3)
    throw invalid_argument("invalid ctxt_size_threshold");

  size_t keys_count = 0;

  unordered_map<ir::Term *, size_t> ctxt_terms_sizes;
  for (auto id : func->get_top_sorted_terms_ids())
  {
    auto term = func->data_flow().get_term(id);
    if (!term)
      continue;

    if (term->type() != ir::Term::Type::cipher)
      continue;

    if (term->is_leaf())
    {
      ctxt_terms_sizes.emplace(term, 2);
      continue;
    }

    vector<size_t> ctxt_args_sizes;
    for (auto operand : term->operands())
    {
      if (operand->type() == ir::Term::Type::cipher)
        ctxt_args_sizes.push_back(ctxt_terms_sizes.at(operand));
    }
    auto ctxt_result_size = get_ctxt_result_size(term->op_code().type(), ctxt_args_sizes);

    auto relin_operands_cond = [&ctxt_result_size, ctxt_size_threshold, term]() {
      return ctxt_result_size > ctxt_size_threshold ||
             (term->op_code().type() == ir::OpCode::Type::rotate && ctxt_result_size > 2);
    };
    if (relin_operands_cond())
    {
      vector<pair<ir::Term *, size_t>> ctxt_terms_with_size;
      ctxt_terms_with_size.reserve(term->operands().size());
      for (size_t i = 0; i < term->operands().size(); ++i)
        ctxt_terms_with_size.push_back({term->operands()[i], ctxt_args_sizes[i]});

      sort(
        ctxt_terms_with_size.begin(), ctxt_terms_with_size.end(),
        [](const pair<ir::Term *, size_t> &lhs, const pair<ir::Term *, size_t> &rhs) {
          return lhs.second > rhs.second;
        });

      for (size_t i = 0; i < ctxt_terms_with_size.size(); ++i)
        ctxt_args_sizes[i] = ctxt_terms_with_size[i].second;

      {
        size_t i = 0;
        while (relin_operands_cond())
        {
          auto relin_operand = func->insert_op_term(ir::OpCode::relin, {ctxt_terms_with_size[i].first});
          keys_count = max(keys_count, ctxt_terms_with_size[i].second - 2);
          func->replace_term_with(ctxt_terms_with_size[i].first, relin_operand);
          ctxt_terms_sizes.erase(ctxt_terms_with_size[i].first);
          ctxt_terms_sizes.emplace(relin_operand, 2);
          ctxt_terms_with_size[i].second = 2;
          ctxt_args_sizes[i] = 2;
          size_t j = i + 1;
          while (j < ctxt_terms_with_size.size() && ctxt_terms_with_size[j].first == ctxt_terms_with_size[i].first)
          {
            ctxt_terms_with_size[j].second = 2;
            ctxt_args_sizes[j] = 2;
            ++j;
          }
          i = j;
          ctxt_result_size = get_ctxt_result_size(term->op_code().type(), ctxt_args_sizes);
        }
      }
    }
    ctxt_terms_sizes.emplace(term, ctxt_result_size);
  }
  return keys_count;
}

size_t relin_after_ctxt_ctxt_mul(const shared_ptr<ir::Func> &func)
{
  for (auto id : func->get_top_sorted_terms_ids())
  {
    auto term = func->data_flow().get_term(id);
    if (!term)
      continue;

    if (is_ctxt_ctxt_mul(term))
    {
      auto relin_term = func->insert_op_term(ir::OpCode::relin, {term});
      func->replace_term_with(term, relin_term);
    }
  }

  return 1;
}

size_t get_ctxt_result_size(ir::OpCode::Type op_code_type, const vector<size_t> &ctxt_args_sizes)
{
  switch (op_code_type)
  {
  case ir::OpCode::Type::encrypt:
    return 2;

  case ir::OpCode::Type::add:
  case ir::OpCode::Type::sub:
    if (ctxt_args_sizes.empty() || ctxt_args_sizes.size() > 2)
      throw invalid_argument("invalid number of ctxt args");

    return *max_element(ctxt_args_sizes.begin(), ctxt_args_sizes.end());

  case ir::OpCode::Type::negate:
  case ir::OpCode::Type::rotate:
    if (ctxt_args_sizes.size() != 1)
      throw invalid_argument("invalid number of ctxt args");

    return ctxt_args_sizes[0];

  case ir::OpCode::Type::square:
    if (ctxt_args_sizes.size() != 1)
      throw invalid_argument("invalid number of ctxt args");

    return 2 * ctxt_args_sizes[0] - 1;

  case ir::OpCode::Type::mul:
    if (ctxt_args_sizes.size() == 1)
      return ctxt_args_sizes[0];

    else if (ctxt_args_sizes.size() == 2)
      return ctxt_args_sizes[0] + ctxt_args_sizes[1] - 1;

    else
      throw invalid_argument("invalid number of ctxt args");

  case ir::OpCode::Type::mod_switch:
    if (ctxt_args_sizes.size() != 1)
      throw invalid_argument("invalid number of ctxt args");

    return ctxt_args_sizes[0];

  case ir::OpCode::Type::relin:
    if (ctxt_args_sizes.size() != 1)
      throw invalid_argument("invalid number of ctxt args");

    return 2;

  default:
    throw invalid_argument("unhandled get ctxt size for operation");
  }
}

bool is_ctxt_ctxt_mul(const ir::Term *term)
{
  if (term->op_code().type() == ir::OpCode::Type::mul)
  {
    auto arg1 = term->operands()[0];
    auto arg2 = term->operands()[1];
    if (arg1->type() == ir::Term::Type::cipher && arg2->type() == ir::Term::Type::cipher)
      return true;

    return false;
  }

  if (term->op_code().type() == ir::OpCode::Type::square)
  {
    auto arg = term->operands()[0];
    if (arg->type() == ir::Term::Type::cipher)
      return true;

    return false;
  }

  return false;
}
} // namespace fheco::passes
