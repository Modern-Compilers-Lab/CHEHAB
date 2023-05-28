#include "fheco/ir/func.hpp"
#include "fheco/trs/rule.hpp"
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

function<bool(const Subst &)> Rule::has_less_ctxt_leaves(TermMatcher x, TermMatcher y)
{
  return [x = move(x), y = move(y)](const Subst &subst) {
    TermsMetric dp;
    auto x_term = subst.get(x);
    auto y_term = subst.get(y);

    int64_t x_ctxt_leaves_count = 0;
    if (x_term->type() == ir::Term::Type::cipher)
    {
      count_ctxt_leaves(x_term, dp);
      x_ctxt_leaves_count = dp.at(x_term);
    }

    int64_t y_ctxt_leaves_count = 0;
    if (y_term->type() == ir::Term::Type::cipher)
    {
      count_ctxt_leaves(y_term, dp);
      y_ctxt_leaves_count = dp.at(y_term);
    }

    return x_ctxt_leaves_count < y_ctxt_leaves_count;
  };
};

function<bool(const Subst &)> Rule::has_less_ctxt_leaves(TermMatcher x, TermMatcher y1, TermMatcher y2)
{
  return [x = move(x), y1 = move(y1), y2 = move(y2)](const Subst &subst) {
    TermsMetric dp;
    auto x_term = subst.get(x);
    auto y1_term = subst.get(y1);
    auto y2_term = subst.get(y2);

    int64_t x_ctxt_leaves_count = 0;
    if (x_term->type() == ir::Term::Type::cipher)
    {
      count_ctxt_leaves(x_term, dp);
      x_ctxt_leaves_count = dp.at(x_term);
    }

    int64_t y1_ctxt_leaves_count = 0;
    if (y1_term->type() == ir::Term::Type::cipher)
    {
      count_ctxt_leaves(y1_term, dp);
      y1_ctxt_leaves_count = dp.at(y1_term);
    }

    int64_t y2_ctxt_leaves_count = 0;
    if (y2_term->type() == ir::Term::Type::cipher)
    {
      count_ctxt_leaves(y2_term, dp);
      y2_ctxt_leaves_count = dp.at(y2_term);
    }

    return x_ctxt_leaves_count < (y1_ctxt_leaves_count + y2_ctxt_leaves_count);
  };
};
} // namespace fheco::trs
