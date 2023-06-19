#include "fheco/ir/func.hpp"
#include "fheco/trs/rule.hpp"
#include "fheco/util/common.hpp"
#include <cstddef>
#include <cstdint>
#include <utility>

using namespace std;

namespace fheco::trs
{
function<bool(const Subst &)> Rule::is_not_const(TermMatcher x, shared_ptr<ir::Func> func)
{
  return [func = move(func), x = move(x)](const Subst &subst) {
    return func->data_flow().is_const(subst.get(x));
  };
}

function<bool(const Subst &)> Rule::is_not_rotation(TermMatcher x)
{
  return [x = move(x)](const Subst &subst) {
    return subst.get(x)->op_code().type() != ir::OpCode::Type::rotate;
  };
}

function<bool(const Subst &)> Rule::has_less_ctxt_leaves(TermMatcher x, TermMatcher y, TermsMetric &dp)
{
  return [x = move(x), y = move(y), &dp](const Subst &subst) {
    TermsMetric dp;
    auto x_term = subst.get(x);
    auto y_term = subst.get(y);

    int64_t x_ctxt_leaves_count = 0;
    if (x_term->type() == ir::Term::Type::cipher)
    {
      count_ctxt_leaves(x_term, dp);
      x_ctxt_leaves_count = dp.at(x_term->id());
    }

    int64_t y_ctxt_leaves_count = 0;
    if (y_term->type() == ir::Term::Type::cipher)
    {
      count_ctxt_leaves(y_term, dp);
      y_ctxt_leaves_count = dp.at(y_term->id());
    }

    return x_ctxt_leaves_count < y_ctxt_leaves_count;
  };
}

function<bool(const Subst &)> Rule::has_less_ctxt_leaves(TermMatcher x, TermMatcher y1, TermMatcher y2, TermsMetric &dp)
{
  return [x = move(x), y1 = move(y1), y2 = move(y2), &dp](const Subst &subst) {
    auto x_term = subst.get(x);
    auto y1_term = subst.get(y1);
    auto y2_term = subst.get(y2);

    int64_t x_ctxt_leaves_count = 0;
    if (x_term->type() == ir::Term::Type::cipher)
    {
      count_ctxt_leaves(x_term, dp);
      x_ctxt_leaves_count = dp.at(x_term->id());
    }

    int64_t y1_ctxt_leaves_count = 0;
    if (y1_term->type() == ir::Term::Type::cipher)
    {
      count_ctxt_leaves(y1_term, dp);
      y1_ctxt_leaves_count = dp.at(y1_term->id());
    }

    int64_t y2_ctxt_leaves_count = 0;
    if (y2_term->type() == ir::Term::Type::cipher)
    {
      count_ctxt_leaves(y2_term, dp);
      y2_ctxt_leaves_count = dp.at(y2_term->id());
    }

    return x_ctxt_leaves_count < (y1_ctxt_leaves_count + y2_ctxt_leaves_count);
  };
}

vector<Rule> Rule::generate_customized_terms_variants() const
{
  if (lhs_.type() != TermMatcherType::term)
    return {};

  if (has_dynamic_rhs_)
    throw invalid_argument("cannot customize rule with dynamic rhs");

  auto vars = get_generic_variables();
  vector<vector<pair<size_t, TermMatcherType>>> vars_possible_types(vars.size());
  size_t i = 0;
  for (const TermMatcher &var : vars)
  {
    vars_possible_types[i] = {{var.id(), TermMatcherType::cipher}, {var.id(), TermMatcherType::plain}};
    ++i;
  }
  auto combinations = util::cart_product(vars_possible_types);
  vector<unordered_map<size_t, TermMatcherType>> indexed_combinations(combinations.size());
  i = 0;
  for (const auto &comb : combinations)
  {
    unordered_map<size_t, TermMatcherType> indexed_comb;
    for (const auto &var_type : comb)
      indexed_comb.emplace(var_type.first, var_type.second);

    indexed_combinations[i] = move(indexed_comb);
    ++i;
  }

  vector<Rule> result(indexed_combinations.size());
  i = 0;
  for (const auto &indexed_comb : indexed_combinations)
  {
    auto variant = *this;
    variant.name_ += "-" + to_string(i + 1);
    variant.lhs_.customize_generic_subterms(indexed_comb);
    auto rhs = get_rhs();
    rhs.customize_generic_subterms(indexed_comb);
    variant.rhs_ = [static_rhs = move(rhs)](const Subst &) {
      return static_rhs;
    };
    result[i] = move(variant);
    ++i;
  }
  return result;
}
} // namespace fheco::trs
