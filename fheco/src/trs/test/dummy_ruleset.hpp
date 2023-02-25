#pragma once
#include "trs.hpp"
#include "trs_util_functions.hpp"

#define CIRCUIT_SAVE_FLAG true

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
  {(x << p), x, p == 0},
  {(x << p) << q, x << MatchingTerm::fold(p + q)},
  /*
  {((x << p) + y) + (z << q), ((x << p) + (z << q)) + y,
   MatchingTerm::opcode_of(y) != static_cast<int>(ir::OpCode::rotate_rows)},
  {(y + (x << p)) + (z << q), ((x << p) + (z << q)) + y,
   MatchingTerm::opcode_of(y) != static_cast<int>(ir::OpCode::rotate_rows)},
  {(x << p) + (y + (z << q)), ((x << p) + (z << q)) + y,
   MatchingTerm::opcode_of(y) != static_cast<int>(ir::OpCode::rotate_rows)},
  {(x << p) + ((z << q) + y), ((x << p) + (z << q)) + y,
   MatchingTerm::opcode_of(y) != static_cast<int>(ir::OpCode::rotate_rows)},
   */
  {(x + (y << p)) + (z << q), x + (((y << MatchingTerm::fold(p - q)) + z) << q),
   (p > q) && (p > 0) && (q > 0) && MatchingTerm::opcode_of(x) != static_cast<int>(ir::OpCode::rotate_rows),
   CIRCUIT_SAVE_FLAG},
  {((y << p) + x) + (z << q), x + (((y << MatchingTerm::fold(p - q)) + z) << q),
   (p > q) && (p > 0) && (q > 0) && MatchingTerm::opcode_of(x) != static_cast<int>(ir::OpCode::rotate_rows),
   CIRCUIT_SAVE_FLAG},
  {(z << q) + (x + (y << p)), x + (((y << MatchingTerm::fold(p - q)) + z) << q),
   (p > q) && (p > 0) && (q > 0) && MatchingTerm::opcode_of(x) != static_cast<int>(ir::OpCode::rotate_rows),
   CIRCUIT_SAVE_FLAG},
  {(z << q) + ((y << p) + x), x + (((y << MatchingTerm::fold(p - q)) + z) << q),
   (p > q) && (p > 0) && (q > 0) && MatchingTerm::opcode_of(x) != static_cast<int>(ir::OpCode::rotate_rows),
   CIRCUIT_SAVE_FLAG},
  {(x + (y << p)) + (z << q), x + (((z << MatchingTerm::fold(p - q)) + y) << q),
   (p < q) && (p > 0) && (q > 0) && MatchingTerm::opcode_of(x) != static_cast<int>(ir::OpCode::rotate_rows),
   CIRCUIT_SAVE_FLAG},
  {((y << p) + x) + (z << q), x + (((z << MatchingTerm::fold(q - p)) + y) << q),
   (p < q) && (p > 0) && (q > 0) && MatchingTerm::opcode_of(x) != static_cast<int>(ir::OpCode::rotate_rows),
   CIRCUIT_SAVE_FLAG},
  {(z << q) + (x + (y << p)), x + (((z << MatchingTerm::fold(q - p)) + y) << q),
   (p < q) && (p > 0) && (q > 0) && MatchingTerm::opcode_of(x) != static_cast<int>(ir::OpCode::rotate_rows),
   CIRCUIT_SAVE_FLAG},
  {(z << q) + ((y << p) + x), x + (((z << MatchingTerm::fold(q - p)) + y) << q),
   (p < q) && (p > 0) && (q > 0) && MatchingTerm::opcode_of(x) != static_cast<int>(ir::OpCode::rotate_rows),
   CIRCUIT_SAVE_FLAG},
  {(x + (y << p)) + (z << q), x + (((y << MatchingTerm::fold(p - q)) + z) << q),
   (p < q) && (p < 0) && (q < 0) && MatchingTerm::opcode_of(x) != static_cast<int>(ir::OpCode::rotate_rows),
   CIRCUIT_SAVE_FLAG},
  {((y << p) + x) + (z << q), x + (((y << MatchingTerm::fold(p - q)) + z) << q),
   (p < q) && (p < 0) && (q < 0) && MatchingTerm::opcode_of(x) != static_cast<int>(ir::OpCode::rotate_rows),
   CIRCUIT_SAVE_FLAG},
  {(z << q) + (x + (y << p)), x + (((y << MatchingTerm::fold(p - q)) + z) << q),
   (p < q) && (p < 0) && (q < 0) && MatchingTerm::opcode_of(x) != static_cast<int>(ir::OpCode::rotate_rows),
   CIRCUIT_SAVE_FLAG},
  {(z << q) + ((y << p) + x), x + (((y << MatchingTerm::fold(p - q)) + z) << q),
   (p < q) && (p < 0) && (q < 0) && MatchingTerm::opcode_of(x) != static_cast<int>(ir::OpCode::rotate_rows),
   CIRCUIT_SAVE_FLAG},
  {(x + (y << p)) + (z << p), x + ((y + z) << p),
   MatchingTerm::opcode_of(x) != static_cast<int>(ir::OpCode::rotate_rows)},
  {((y << p) + x) + (z << p), x + ((y + z) << p),
   MatchingTerm::opcode_of(x) != static_cast<int>(ir::OpCode::rotate_rows)},
  {(z << p) + (x + (y << p)), x + ((z + y) << p),
   MatchingTerm::opcode_of(x) != static_cast<int>(ir::OpCode::rotate_rows)},
  {(z << p) + ((y << p) + x), x + ((z + y) << p),
   MatchingTerm::opcode_of(x) != static_cast<int>(ir::OpCode::rotate_rows)},
  {(x << p) + (y << p), (x + y) << p},
  {(x << p) * (y << p), (x * y) << p},
  {(x << p) - (y << p), (x - y) << p},
  {(x << p) + (y << q), ((x << MatchingTerm::fold(p - q)) + y) << q, (p > q) && (p > 0) && (q > 0), CIRCUIT_SAVE_FLAG},
  {(x << p) + (y << q), ((y << MatchingTerm::fold(q - p)) + x) << p, (q > p) && (p > 0) && (q > 0), CIRCUIT_SAVE_FLAG},
  {(x << p) + (y << q), ((x << MatchingTerm::fold(p - q)) + y) << q, (p < q) && (p > 0) && (q > 0), CIRCUIT_SAVE_FLAG},
  {(x << p) + (y << q), ((y << MatchingTerm::fold(q - p)) + x) << p, (q < p) && (p > 0) && (q > 0), CIRCUIT_SAVE_FLAG},
  {(x << p) + (y << q), ((x << MatchingTerm::fold(p - q)) + y) << q, (p < q) && (p < 0) && (q < 0), CIRCUIT_SAVE_FLAG},
  {(x << p) + (y << q), ((y << MatchingTerm::fold(q - p)) + x) << p, (q < p) && (p < 0) && (q < 0), CIRCUIT_SAVE_FLAG},
  {(x << p) + (y << q), ((x << MatchingTerm::fold(p - q)) + y) << q, (p < q) && (p < 0) && (q < 0), CIRCUIT_SAVE_FLAG},
  {(x << p) + (y << q), ((y << MatchingTerm::fold(q - p)) + x) << p, (q < p) && (p < 0) && (q < 0), CIRCUIT_SAVE_FLAG}};

/*

  {(x + (y << p)) + (z << q), x + (((y << MatchingTerm::fold(p - q)) + z) << q), p>q},
  {((y << p) + x) + (z << q), x + (((y << MatchingTerm::fold(p - q)) + z) << q), p>q},
  {(z << q) + (x + (y << p)), x + (((y << MatchingTerm::fold(p - q)) + z) << q), p>q},
  {(z << q) + ((y << p) + x), x + (((y << MatchingTerm::fold(p - q)) + z) << q), p>q},

*/

std::vector<RewriteRule> dummy_ruleset2 = {
  {(x << p), x, p == 0},
  {(x << p) + (y << p), (x + y) << p},
  {(x << p) * (y << p), (x * y) << p},
  {(x << p) - (y << p), (x - y) << p}};

MatchingTerm plain1(fheco_trs::TermType::plaintextType);
MatchingTerm plain2(fheco_trs::TermType::plaintextType);
/*
(x - (y + z)) + z => (x - y)
*/
std::vector<RewriteRule> dummy_ruleset3 = {
  {(x * y) * x, y *square(x)},
  {(x - (y + z)) + z, x - y},
  {(((plain1 - x) - y) + plain2), (MatchingTerm::fold(plain1 + plain2) - y) - x,
   MatchingTerm::isconst(plain1) && MatchingTerm::isconst(plain2)},
  {(x + plain1) - plain2, x + MatchingTerm::fold(plain1 - plain2),
   MatchingTerm::isconst(plain1) && MatchingTerm::isconst(plain2)},
  {(x - n), x + MatchingTerm::fold(0 - n)},
  {(x + plain1) + plain2, x + MatchingTerm::fold(plain1 + plain2),
   MatchingTerm::isconst(plain1) && MatchingTerm::isconst(plain2)},
  {(x * plain1) * plain2, x *MatchingTerm::fold(plain1 *plain2),
   MatchingTerm::isconst(plain1) && MatchingTerm::isconst(plain2)}};
} // namespace fheco_trs
