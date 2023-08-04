#include "fheco/ir/func.hpp"
#include "fheco/trs/rule.hpp"
#include "fheco/util/common.hpp"
#include <cstddef>
#include <cstdint>
#include <utility>

using namespace std;

namespace fheco::trs
{
function<bool(const Substitution &)> Rule::is_not_const(TermMatcher x, shared_ptr<ir::Func> func)
{
  return [func = move(func), x = move(x)](const Substitution &subst) {
    return func->data_flow().is_const(subst.get(x));
  };
}

function<bool(const Substitution &)> Rule::is_not_rotation(TermMatcher x)
{
  return [x = move(x)](const Substitution &subst) {
    return subst.get(x)->op_code().type() != ir::OpCode::Type::rotate;
  };
}

function<bool(const Substitution &)> Rule::has_less_ctxt_leaves(TermMatcher x, TermMatcher y, TermsMetric &cache)
{
  return [x = move(x), y = move(y), &cache](const Substitution &subst) {
    TermsMetric cache;
    auto x_term = subst.get(x);
    auto y_term = subst.get(y);

    int64_t x_ctxt_leaves_count = 0;
    if (x_term->type() == ir::Term::Type::cipher)
    {
      count_ctxt_leaves(x_term, cache);
      x_ctxt_leaves_count = cache.at(x_term->id());
    }

    int64_t y_ctxt_leaves_count = 0;
    if (y_term->type() == ir::Term::Type::cipher)
    {
      count_ctxt_leaves(y_term, cache);
      y_ctxt_leaves_count = cache.at(y_term->id());
    }

    return x_ctxt_leaves_count < y_ctxt_leaves_count;
  };
}

function<bool(const Substitution &)> Rule::has_less_ctxt_leaves(
  TermMatcher x, TermMatcher y1, TermMatcher y2, TermsMetric &cache)
{
  return [x = move(x), y1 = move(y1), y2 = move(y2), &cache](const Substitution &subst) {
    auto x_term = subst.get(x);
    auto y1_term = subst.get(y1);
    auto y2_term = subst.get(y2);

    int64_t x_ctxt_leaves_count = 0;
    if (x_term->type() == ir::Term::Type::cipher)
    {
      count_ctxt_leaves(x_term, cache);
      x_ctxt_leaves_count = cache.at(x_term->id());
    }

    int64_t y1_ctxt_leaves_count = 0;
    if (y1_term->type() == ir::Term::Type::cipher)
    {
      count_ctxt_leaves(y1_term, cache);
      y1_ctxt_leaves_count = cache.at(y1_term->id());
    }

    int64_t y2_ctxt_leaves_count = 0;
    if (y2_term->type() == ir::Term::Type::cipher)
    {
      count_ctxt_leaves(y2_term, cache);
      y2_ctxt_leaves_count = cache.at(y2_term->id());
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
  vector<vector<pair<size_t, TermMatcherType>>> vars_possible_types;
  vars_possible_types.reserve(vars.size());
  for (const TermMatcher &var : vars)
    vars_possible_types.push_back({{var.id(), TermMatcherType::cipher}, {var.id(), TermMatcherType::plain}});

  auto combinations = util::cart_product(vars_possible_types);
  vector<unordered_map<size_t, TermMatcherType>> indexed_combinations;
  indexed_combinations.reserve(combinations.size());
  for (const auto &comb : combinations)
  {
    unordered_map<size_t, TermMatcherType> indexed_comb;
    for (const auto &var_type : comb)
      indexed_comb.emplace(var_type.first, var_type.second);

    indexed_combinations.push_back(move(indexed_comb));
  }

  vector<Rule> result;
  result.reserve(indexed_combinations.size());
  size_t i = 0;
  for (const auto &indexed_comb : indexed_combinations)
  {
    auto variant = *this;
    variant.name_ += "-" + to_string(i + 1);
    variant.lhs_.customize_generic_subterms(indexed_comb);
    auto rhs = get_rhs();
    rhs.customize_generic_subterms(indexed_comb);
    variant.rhs_ = [static_rhs = move(rhs)](const Substitution &) {
      return static_rhs;
    };
    result.push_back(move(variant));
    ++i;
  }
  return result;
}
} // namespace fheco::trs
