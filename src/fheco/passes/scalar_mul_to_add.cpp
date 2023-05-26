#include "fheco/ir/func.hpp"
#include "fheco/trs/common.hpp"
#include "fheco/trs/ops_overloads.hpp"
#include "fheco/trs/trs.hpp"
#include "fheco/passes/scalar_mul_to_add.hpp"
#include "fheco/util/common.hpp"
#include <cstdlib>
#include <vector>

using namespace std;

namespace fheco::passes
{
void convert_scalar_mul_to_add(const std::shared_ptr<ir::Func> &func, size_t scalar_threshold)
{
  trs::TermMatcher c_x{trs::TermMatcherType::cipher, "ctxt_x"};

  trs::TermMatcher const1{trs::TermMatcherType::const_, "const1"};

  auto gen_balan_add = [&c_x, &const1, &func](const trs::Subst &subst) {
    auto const_term = subst.get(const1);
    auto scalar_val = (*func->data_flow().get_const_val(const_term))[0];
    bool is_negative = scalar_val < 0;
    size_t uscalar_val = abs(scalar_val);

    trs::TermMatcher elt = c_x;
    for (size_t i = 1; i < uscalar_val; ++i)
      elt = elt + c_x;

    return is_negative ? -elt : elt;
  };

  auto const_scalar_cond = [&const1, &func, scalar_threshold](const trs::Subst &subst) {
    auto const_term = subst.get(const1);
    auto const_val = *func->data_flow().get_const_val(const_term);
    return abs(const_val[0]) <= scalar_threshold && util::is_scalar(const_val);
  };

  vector<trs::Rule> scalar_mul_to_add_rules{
    {"ctxt-mul-scalar", c_x * const1, gen_balan_add, const_scalar_cond},
    {"scalar-ctxt-mul", const1 * c_x, gen_balan_add, const_scalar_cond}};

  trs::TRS trs = trs::TRS::make_void_trs(func);
  for (auto id : func->get_top_sorted_terms_ids())
  {
    auto term = func->data_flow().get_term(id);
    if (!term)
      continue;

    for (const auto &rule : scalar_mul_to_add_rules)
    {
      if (trs.apply_rule(term, rule))
        break;
    }
  }
}
} // namespace fheco::passes
