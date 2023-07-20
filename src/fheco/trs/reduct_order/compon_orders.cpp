#include "fheco/trs/fold_op_gen_matcher.hpp"
#include "fheco/trs/ops_overloads.hpp"
#include "fheco/trs/reduct_order/compon_orders.hpp"
#include "fheco/util/expr_printer.hpp"
#include <iostream>

using namespace std;

namespace fheco::trs
{
CompResult sum_xdepth_order(const TermMatcher &lhs, const TermMatcher &rhs)
{
  auto lhs_sum_xdepths = sum_cipher_leaves_xdepths(lhs);
  auto rhs_sum_xdepths = sum_cipher_leaves_xdepths(rhs);

  if (lhs_sum_xdepths < rhs_sum_xdepths)
    return CompResult::less;

  auto lhs_vars_xdepths = compute_cipher_vars_xdepths(lhs);
  auto rhs_vars_xdepths = compute_cipher_vars_xdepths(rhs);
  if (terms_feature_values_order(lhs_vars_xdepths, rhs_vars_xdepths) == CompResult::less)
    return CompResult::not_generalizable;

  auto lhs_cipher_vars_occ = count_class_vars_occ(lhs, &is_cipher);
  auto rhs_cipher_vars_occ = count_class_vars_occ(rhs, &is_cipher);
  if (terms_feature_values_order(lhs_cipher_vars_occ, rhs_cipher_vars_occ) == CompResult::less)
    return CompResult::not_generalizable;

  return lhs_sum_xdepths > rhs_sum_xdepths ? CompResult::greater : CompResult::equal;
}

CompResult sum_depth_order(const TermMatcher &lhs, const TermMatcher &rhs)
{
  auto lhs_sum_depths = sum_cipher_leaves_depths(lhs);
  auto rhs_sum_depths = sum_cipher_leaves_depths(rhs);
  if (lhs_sum_depths < rhs_sum_depths)
    return CompResult::less;

  auto lhs_vars_depths = compute_cipher_vars_depths(lhs);
  auto rhs_vars_depths = compute_cipher_vars_depths(rhs);
  if (terms_feature_values_order(lhs_vars_depths, rhs_vars_depths) == CompResult::less)
    return CompResult::not_generalizable;

  auto lhs_cipher_vars_occ = count_class_vars_occ(lhs, &is_cipher);
  auto rhs_cipher_vars_occ = count_class_vars_occ(rhs, &is_cipher);
  if (terms_feature_values_order(lhs_cipher_vars_occ, rhs_cipher_vars_occ) == CompResult::less)
    return CompResult::not_generalizable;

  return lhs_sum_depths > rhs_sum_depths ? CompResult::greater : CompResult::equal;
}

CompResult sum_total_depth_order(const TermMatcher &lhs, const TermMatcher &rhs)
{
  auto lhs_sum_depths = sum_leaves_depths(lhs);
  auto rhs_sum_depths = sum_leaves_depths(rhs);
  if (lhs_sum_depths < rhs_sum_depths)
    return CompResult::less;

  auto lhs_vars_depths = compute_vars_depths(lhs);
  auto rhs_vars_depths = compute_vars_depths(rhs);
  if (terms_feature_values_order(lhs_vars_depths, rhs_vars_depths) == CompResult::less)
    return CompResult::not_generalizable;

  auto lhs_vars_occ = count_class_vars_occ(lhs, &is_var);
  auto rhs_vars_occ = count_class_vars_occ(rhs, &is_var);
  if (terms_feature_values_order(lhs_vars_occ, rhs_vars_occ) == CompResult::less)
    return CompResult::not_generalizable;

  return lhs_sum_depths > rhs_sum_depths ? CompResult::greater : CompResult::equal;
}

CompResult class_subterms_count_order(
  const TermMatcher &lhs, const TermMatcher &rhs, const TermClassChecker &class_checker,
  const TermClassChecker &vars_class_checker)
{
  auto lhs_count = count_class_subterms(lhs, class_checker);
  auto rhs_count = count_class_subterms(rhs, class_checker);
  if (lhs_count < rhs_count)
    return CompResult::less;

  auto lhs_cipher_vars_occ = count_class_vars_occ(lhs, vars_class_checker);
  auto rhs_cipher_vars_occ = count_class_vars_occ(rhs, vars_class_checker);
  if (terms_feature_values_order(lhs_cipher_vars_occ, rhs_cipher_vars_occ) == CompResult::less)
    return CompResult::not_generalizable;

  return lhs_count > rhs_count ? CompResult::greater : CompResult::equal;
}

CompResult phi_str_order(
  const TermMatcher &lhs, const TermMatcher &rhs, const TermClassChecker &class_a_checker,
  const TermClassChecker &class_b_checker)
{
  auto lhs_str = make_phi_str(lhs, class_a_checker, class_b_checker);
  auto rhs_str = make_phi_str(rhs, class_a_checker, class_b_checker);
  if (lhs_str < rhs_str)
    return CompResult::less;

  auto lhs_cipher_vars_occ = count_class_vars_occ(lhs, &is_cipher);
  auto rhs_cipher_vars_occ = count_class_vars_occ(rhs, &is_cipher);
  if (terms_feature_values_order(lhs_cipher_vars_occ, rhs_cipher_vars_occ) == CompResult::less)
    return CompResult::not_generalizable;

  return lhs_str > rhs_str ? CompResult::greater : CompResult::equal;
}

CompResult sum_rotation_steps_order(const TermMatcher &lhs, const TermMatcher &rhs)
{
  auto lhs_sum = sum_rotation_steps(lhs);
  auto rhs_sum = sum_rotation_steps(rhs);

  auto lhs_cipher_vars_occ = count_class_vars_occ(lhs, &is_cipher);
  auto rhs_cipher_vars_occ = count_class_vars_occ(rhs, &is_cipher);
  if (terms_feature_values_order(lhs_cipher_vars_occ, rhs_cipher_vars_occ) == CompResult::less)
    return CompResult::not_generalizable;

  if (lhs_sum.id() && rhs_sum.id())
  {
    auto diff = fold_symbolic_op_gen_matcher(lhs_sum - rhs_sum);
    if (diff.id())
    {
      clog << "under the condition " << util::ExprPrinter::make_op_gen_matcher_str_expr(diff) << " > 0\n";
      return CompResult::greater;
    }
    return CompResult::equal;
  }

  if (lhs_sum.id() || rhs_sum.id())
  {
    clog << "under the condition ";
    if (lhs_sum.id())
      clog << util::ExprPrinter::make_op_gen_matcher_str_expr(fold_symbolic_op_gen_matcher(lhs_sum));
    else
      clog << '0';

    clog << " > ";

    if (rhs_sum.id())
      clog << util::ExprPrinter::make_op_gen_matcher_str_expr(fold_symbolic_op_gen_matcher(rhs_sum));
    else
      clog << '0';

    clog << '\n';
    return CompResult::greater;
  }
  return CompResult::equal;
}
} // namespace fheco::trs
