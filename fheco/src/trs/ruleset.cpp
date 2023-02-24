#include "ruleset.hpp"

using namespace fheco_trs;

MatchingTerm x(fheco_trs::TermType::ciphertextType);
MatchingTerm y(fheco_trs::TermType::ciphertextType);
MatchingTerm z(fheco_trs::TermType::ciphertextType);
MatchingTerm k(fheco_trs::TermType::ciphertextType);
MatchingTerm n(fheco_trs::TermType::scalarType);
MatchingTerm m(fheco_trs::TermType::scalarType);
MatchingTerm p(fheco_trs::TermType::rawDataType);
MatchingTerm q(fheco_trs::TermType::rawDataType);
MatchingTerm r(fheco_trs::TermType::rawDataType);
MatchingTerm s(fheco_trs::TermType::rawDataType);

std::vector<RewriteRule> Ruleset::rules = {
  {(x << p) + (y << q), ((x << MatchingTerm::fold(p - q)) + y) << q, (p > q)},
  {(x << p) + (y << q), ((y << MatchingTerm::fold(q - p)) + x) << p, (q > p)},
  {(x << p), x, p == 0}};
