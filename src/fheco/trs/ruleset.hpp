#pragma once

#include "trs.hpp"
#include "trs_util_functions.hpp"
#include <functional>
#include <vector>

namespace fheco_trs
{
class Ruleset
{
public:
  static std::vector<RewriteRule> mul_rules;
  static std::vector<RewriteRule> rotate_rules;
  static std::vector<RewriteRule> add_rules;
  static std::vector<RewriteRule> sub_rules;
  static std::vector<RewriteRule> misc_rules;

private:
  static MatchingTerm x;
  static MatchingTerm y;
  static MatchingTerm z;
  static MatchingTerm u;
  static MatchingTerm a;
  static MatchingTerm n;
  static MatchingTerm m;

  static std::function<MatchingTerm(const ir::Program *prgm, const RewriteRule::matching_term_ir_node_map &)>
  gen_rhs_scalar_mul_to_sum(const MatchingTerm &a, const MatchingTerm &x);
};
} // namespace fheco_trs
