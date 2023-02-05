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
MatchingTerm r(fheco_trs::TermType::rawDataType);
MatchingTerm s(fheco_trs::TermType::rawDataType);

/*
std::vector<RewriteRule> dummy_ruleset = {
  {(x + n) - m, x + MatchingTerm::fold(n - m)},
  {x + n, x, n == 0},
  {(x + y * n) + y * m, x + (y * MatchingTerm::fold(n + m))},
  {(x << p) << q, x << (MatchingTerm::fold(p + q))},
  {x + (y << p) + (z << q), x + ((y << p) + (z << q))},
  {(y << p) + (z << q) + x, x + ((y << p) + (z << q))},
  {x + (y << p) + (z << q) + (k << r), x + ((y << p) + (z << q) + (k << r))},
  {(x << p) + (y << p), (x + y) << p},
  {(x << p) * (y << p), (x * y) << p},
  {(x << p) - (y << p), (x - y) << p}};
// {(x << p) + (y << q), ((x << MatchingTerm::fold(p - q)) + x) << q, p > q},
// {(x << p) + (y << q), ((y << MatchingTerm::fold(q - p)) + x) << p, q > p},
// {(x << p) * (y << q), ((x << MatchingTerm::fold(p - q)) * y) << q, p > q},
// {(x << p) * (y << q), ((y << MatchingTerm::fold(q - p)) * x) << p, q > p},
// {(x << p) - (y << q), ((x << MatchingTerm::fold(p - q)) - y) << q, p > q},
// {(x << p) - (y << q), ((x - (y << MatchingTerm::fold(q - p)))) << p, q > p}};
*/

/*
std::vector<RewriteRule> dummy_ruleset = {
  {y + (x << p), (x << p) + y, !MatchingTerm::is_opcode_equal_to(y, OpCode::rotate_rows)},
  {(z << q) + ((x << p) + y), (x << p) + (z << q) + y, !MatchingTerm::is_opcode_equal_to(y, OpCode::rotate_rows)},
  {(x << p), x, p == 0},
  {(x << p) << q, x << (MatchingTerm::fold(p + q))},
  {(x << p) + (y << q), (x + y) << p, (p == q)},
  {(x << p) + (y << q), ((x << MatchingTerm::fold(p - q)) + y) << q,
   (p > q) && (p > 0) && (q > 0) && MatchingTerm::is_opcode_equal_to(x, OpCode::undefined) &&
     MatchingTerm::is_opcode_equal_to(y, OpCode::undefined)},
  {(x << p) + (y << q), ((y << MatchingTerm::fold(q - p)) + x) << p,
   (q > p) && (p > 0) && (q > 0) && MatchingTerm::is_opcode_equal_to(y, OpCode::undefined) &&
     MatchingTerm::is_opcode_equal_to(x, OpCode::undefined)},
  {(x << p) + (y << q), ((x << MatchingTerm::fold(p - q)) + y) << q,
   (p < q) && (p < 0) && (q < 0) && MatchingTerm::is_opcode_equal_to(y, OpCode::undefined) &&
     MatchingTerm::is_opcode_equal_to(x, OpCode::undefined)},
  {(x << p) + (y << q), ((y << MatchingTerm::fold(q - p)) + x) << p,
   (q < p) && (p < 0) && (q < 0) && MatchingTerm::is_opcode_equal_to(y, OpCode::undefined) &&
     MatchingTerm::is_opcode_equal_to(x, OpCode::undefined)}};
*/

std::vector<RewriteRule> dummy_ruleset = {
  {y + (x << p), (x << p) + y, !MatchingTerm::is_opcode_equal_to(y, OpCode::rotate_rows)},
  {(z << q) + ((x << p) + y), (x << p) + (z << q) + y, !MatchingTerm::is_opcode_equal_to(y, OpCode::rotate_rows)},
  {(x << p), x, p == 0},
  {(x << p) << q, x << (MatchingTerm::fold(p + q))},
  {x + (y << p) + (z << q), x + ((y << p) + (z << q)), !MatchingTerm::is_opcode_equal_to(x, OpCode::rotate_rows)},
  {(y << p) + (z << q) + x, x + ((y << p) + (z << q)), !MatchingTerm::is_opcode_equal_to(x, OpCode::rotate_rows)},
  {x + (y << p) + (z << q) + (k << r),
   x + ((y << p) + (z << q) + (k << r), !MatchingTerm::is_opcode_equal_to(x, OpCode::rotate_rows))},
  {(x << p) + (y << p), (x + y) << p},
  {(x << p) * (y << p), (x * y) << p},
  {(x << p) - (y << p), (x - y) << p},
  {(x << p) + (y << q), ((x << MatchingTerm::fold(p - q)) + y) << q, (p > q) && (p > 0) && (q > 0)},
  {(x << p) + (y << q), ((y << MatchingTerm::fold(q - p)) + x) << p, (q > p) && (p > 0) && (q > 0)},
  {(x << p) + (y << q), ((x << MatchingTerm::fold(p - q)) + y) << q, (p < q) && (p < 0) && (q < 0)},
  {(x << p) + (y << q), ((y << MatchingTerm::fold(q - p)) + x) << p, (q < p) && (p < 0) && (q < 0)}};

MatchingTerm plain1(fheco_trs::TermType::plaintextType);
MatchingTerm plain2(fheco_trs::TermType::plaintextType);

std::vector<RewriteRule> dummy_ruleset2 = {{(x + plain1) + plain2, x + MatchingTerm::fold(plain1 + plain2)}};

} // namespace fheco_trs
