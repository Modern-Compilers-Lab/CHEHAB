#pragma once
#include "trs.hpp"

namespace fheco_trs
{

MatchingTerm x(fheco_trs::TermType::ciphertextType);
MatchingTerm y(fheco_trs::TermType::ciphertextType);
MatchingTerm z(fheco_trs::TermType::ciphertextType);
MatchingTerm k(fheco_trs::TermType::ciphertextType);
MatchingTerm n(fheco_trs::TermType::scalarType);
MatchingTerm m(fheco_trs::TermType::scalarType);
MatchingTerm p(fheco_trs::TermType::rawDataType);
MatchingTerm q(fheco_trs::TermType::rawDataType);

/*
  {exponentiate(x, p) * exponentiate(y, q), exponentiate(x *y, p), p == q},
*/

/*
  {x * y * z * k, (x * y) * (z * k)},
  {x + x, static_cast<int64_t>(2) * x},
  {n * x, x *n},
  {x + n, x + n},
  {(x + n) + m, x + MatchingTerm::fold(n + m)},
  {exponentiate(x, p) * exponentiate(x, q), exponentiate(x, MatchingTerm::fold(p + q))}};*/

/*

  {x * y * z * k, (x * y) * (z * k)},
  {x + x, static_cast<int64_t>(2) * x},
  {(x + n) + m, x + MatchingTerm::fold(n + m)},
  {(x + n), (y + m), (x + y) + MatchingTerm::fold(n + m)},
  {(x + n), (m + y), (x + y) + MatchingTerm::fold(n + m)},
  {exponentiate(x, p) * exponentiate(x, q), exponentiate(x, MatchingTerm::fold(p + q))}};*/

std::vector<RewriteRule> dummy_ruleset = {
  {(x + n) - m, x + MatchingTerm::fold(n - m)}, {x + n, x, n == static_cast<int64_t>(0)}};
} // namespace fheco_trs
