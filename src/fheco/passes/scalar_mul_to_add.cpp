#include "fheco/ir/func.hpp"
#include "fheco/trs/common.hpp"
#include "fheco/trs/ops_overloads.hpp"
#include "fheco/trs/trs.hpp"
#include "fheco/passes/scalar_mul_to_add.hpp"
#include "fheco/util/common.hpp"
#include "fheco/util/expr_printer.hpp"
#include <bitset>
#include <cstdlib>
#include <limits>
#include <vector>

using namespace std;

namespace fheco::passes
{
void convert_scalar_mul_to_add(const std::shared_ptr<ir::Func> &func, size_t conv_threshold)
{
  trs::TermMatcher c_x{trs::TermMatcherType::cipher, "ctxt_x"};

  trs::TermMatcher const1{trs::TermMatcherType::const_, "const1"};

  auto gen_balan_add = [&c_x, &const1, &func](const trs::Substitution &subst) -> trs::TermMatcher {
    auto const_term = subst.get(const1);
    auto scalar_val = (*func->data_flow().get_const_val(const_term))[0];
    bool is_neg = scalar_val < 0;
    size_t uscalar_val = abs(scalar_val);
    size_t next_power_of_two = util::next_power_of_two(uscalar_val);
    if (uscalar_val == next_power_of_two)
    {
      vector<trs::TermMatcher> elts;
      elts.reserve(uscalar_val);
      for (size_t i = 0; i < uscalar_val; ++i)
        elts.push_back(c_x);
      return is_neg ? -trs::balanced_op(elts, trs::TermOpCode::add) : trs::balanced_op(elts, trs::TermOpCode::add);
    }
    size_t prev_power_of_two = next_power_of_two >> 1;
    vector<trs::TermMatcher> elts;
    elts.reserve(prev_power_of_two);
    for (size_t i = 0; i < prev_power_of_two; ++i)
      elts.push_back(c_x);
    auto largest_balanced_sum = trs::balanced_op(elts, trs::TermOpCode::add);
    bitset<numeric_limits<size_t>::digits> bs(uscalar_val);
    int i_msb = util::msb_index(uscalar_val);
    elts.clear();
    elts.reserve(bs.count());
    elts.push_back(largest_balanced_sum);
    auto curr_elt = largest_balanced_sum;
    for (int i = i_msb - 1; i >= 0; --i)
    {
      // self copy assign causes problems
      // also maybe TermMatcher operands as values is costly
      auto tmp = curr_elt;
      curr_elt = tmp.operands()[0];
      if (bs[i])
        elts.push_back(curr_elt);
    }
    return is_neg ? -trs::balanced_op(elts, trs::TermOpCode::add) : trs::balanced_op(elts, trs::TermOpCode::add);
  };

  auto const_scalar_cond = [&const1, &func, conv_threshold](const trs::Substitution &subst) -> bool {
    auto const_term = subst.get(const1);
    auto const_info = *func->data_flow().get_const_info(const_term);
    if (!const_info.is_scalar_)
      return false;

    size_t uscalar_val = abs(const_info.val_[0]);
    if (uscalar_val < 2)
      return false;

    size_t next_power_of_two = util::next_power_of_two(uscalar_val);
    if (uscalar_val == next_power_of_two)
      return util::get_power_of_two(next_power_of_two) <= conv_threshold;

    size_t balanced_add_count = util::get_power_of_two(next_power_of_two) - 1;
    size_t add_count = balanced_add_count + util::popcount(uscalar_val) - 1;
    return add_count <= conv_threshold;
  };

  vector<trs::Rule> scalar_mul_to_add_rules{
    {"ctxt-mul-scalar", c_x * const1, gen_balan_add, const_scalar_cond},
    {"scalar-ctxt-mul", const1 * c_x, gen_balan_add, const_scalar_cond}};

  trs::TRS trs{func};
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
