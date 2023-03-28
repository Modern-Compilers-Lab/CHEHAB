#pragma once
#include "trs.hpp"
#include "trs_util_functions.hpp"

#define CIRCUIT_SAVE_FLAG true

namespace fheco_trs
{

namespace ruleset
{
  using T = MatchingTerm;

  T x(TermType::variable);
  T y(TermType::variable);
  T z(TermType::variable);
  T k(TermType::variable);
  T n(TermType::constant);
  T m(TermType::constant);
  T p(TermType::rawData);
  T q(TermType::rawData);
  T c0(TermType::constant);
  T c1(TermType::constant);
  T t1(TermType::ciphertext);
  T t2(TermType::ciphertext);
  T t3(TermType::ciphertext);
  T t4(TermType::ciphertext);
  T w(TermType::variable);
  T u(TermType::variable);

  std::vector<RewriteRule> rotations_ruleset = {};

  std::vector<RewriteRule> mul_ruleset = {};

  std::vector<RewriteRule> sub_ruleset = {};

  std::vector<RewriteRule> add_ruleset = {};

} // namespace ruleset

} // namespace fheco_trs
