#pragma once

#include "trs.hpp"
#include "trs_util_functions.hpp"
#include <functional>
#include <vector>

namespace fheco_trs
{
class Ruleset
{
  static MatchingTerm x;
  static MatchingTerm y;
  static MatchingTerm z;
  static MatchingTerm u;
  static MatchingTerm a;
  static MatchingTerm n;
  static MatchingTerm m;

  static std::function<MatchingTerm(const ir::Program *prgm, const RewriteRule::matching_term_ir_node_map &)>
  gen_rhs_scalar_mul_to_sum(const MatchingTerm &a, const MatchingTerm &x);

public:
  static std::vector<RewriteRule> rules;
};
} // namespace fheco_trs
