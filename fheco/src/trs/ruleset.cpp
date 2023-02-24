#include "ruleset.hpp"

using namespace fheco_trs;

MatchingTerm x("x", TermType::ciphertextType);
MatchingTerm y("y", TermType::ciphertextType);
MatchingTerm z("z", TermType::ciphertextType);
MatchingTerm u("u", TermType::ciphertextType);

MatchingTerm a("a", TermType::scalarType);

MatchingTerm n("n", fheco_trs::TermType::rawDataType);
MatchingTerm m("m", fheco_trs::TermType::rawDataType);

std::vector<RewriteRule> Ruleset::rules = {
  {(x << n), x, n == 0},
  {x << n << m, x << MatchingTerm::fold((n + m))},
  {(x << n) + (y << n), (x + y) << n},
  {(x << n) + (y << m), (x + (y << MatchingTerm::fold(m - n))) << n, n < m},
  {(x << n) + (y << m), ((x << MatchingTerm::fold(n - m)) + y) << m, n > m}};
