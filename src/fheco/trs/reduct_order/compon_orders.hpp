#pragma once

#include "fheco/trs/reduct_order/common.hpp"

namespace fheco::trs
{
CompResult sum_xdepth_order(const TermMatcher &lhs, const TermMatcher &rhs);

CompResult sum_depth_order(const TermMatcher &lhs, const TermMatcher &rhs);

CompResult sum_total_depth_order(const TermMatcher &lhs, const TermMatcher &rhs);

CompResult class_subterms_count_order(
  const TermMatcher &lhs, const TermMatcher &rhs, const TermClassChecker &class_checker,
  const TermClassChecker &vars_class_checker);

CompResult phi_str_order(
  const TermMatcher &lhs, const TermMatcher &rhs, const TermClassChecker &class_a_checker,
  const TermClassChecker &class_b_checker);

CompResult sum_rotation_steps_order(const TermMatcher &lhs, const TermMatcher &rhs);
} // namespace fheco::trs
