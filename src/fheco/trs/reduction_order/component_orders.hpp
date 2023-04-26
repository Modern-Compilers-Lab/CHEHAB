#pragma once

#include "matching_term.hpp"
#include "utils.hpp"
#include <functional>

namespace fheco_trs
{
relation_type xdepth_order(const MatchingTerm &lhs, const MatchingTerm &rhs);

relation_type he_op_class_order(
  const MatchingTerm &lhs, const MatchingTerm &rhs, const std::function<bool(const MatchingTerm &)> &op_class_checker);

relation_type he_rotation_steps_order(const MatchingTerm &lhs, const MatchingTerm &rhs);

relation_type leaves_class_order(
  const MatchingTerm &lhs, const MatchingTerm &rhs, const std::function<bool(const MatchingTerm &)> &leaf_checker);

} // namespace fheco_trs