#include "fheco/ir/func.hpp"
#include "fheco/passes/insert_relin.hpp"
#include <algorithm>
#include <stdexcept>
#include <unordered_map>

using namespace std;

namespace fheco::passes
{
void insert_relin_ops(const shared_ptr<ir::Func> &func, size_t ctxt_size_threshold)
{
  if (ctxt_size_threshold < 2)
    throw invalid_argument("invalid ctxt_size_threshold");

  unordered_map<ir::Term *, size_t> ctxt_terms_sizes;
  for (auto id : func->get_top_sorted_terms_ids())
  {
    auto term = func->data_flow().get_term(id);
    if (!term)
      continue;

    if (term->type() != ir::Term::Type::cipher)
      continue;

    if (term->is_leaf())
      ctxt_terms_sizes.emplace(term, 2);
    else
    {
      vector<size_t> ctxt_args_sizes;
      for (auto operand : term->operands())
      {
        if (operand->type() == ir::Term::Type::cipher)
          ctxt_args_sizes.push_back(ctxt_terms_sizes.at(operand));
      }
      auto ctxt_result_size = get_ctxt_result_size(term->op_code().type(), ctxt_args_sizes);
      ctxt_terms_sizes.emplace(term, ctxt_result_size);
      if (ctxt_result_size > ctxt_size_threshold)
      {
        ir::Term *relin_term = func->insert_op_term(ir::OpCode::relin, {term});
        func->replace_term_with(term, relin_term);
        ctxt_terms_sizes.emplace(relin_term, 2);
      }
    }
  }
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
} // namespace fheco::passes
