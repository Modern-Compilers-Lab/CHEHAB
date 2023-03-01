#pragma once
#include "trs.hpp"
#include "trs_util_functions.hpp"

#define CIRCUIT_SAVE_FLAG true

namespace fheco_trs
{

using T = MatchingTerm;

T x(TermType::variable);
T y(TermType::variable);
T z(TermType::variable);
T k(TermType::variable);
T n(TermType::constant);
T m(TermType::constant);
T p(TermType::rawDataType);
T q(TermType::rawDataType);

std::vector<RewriteRule> dummy_ruleset = {
  {(x << p), x, p == 0},
  {(x << p) << q, x << T::fold(p + q)},
  {(x + (y << p)) + (z << q), x + (((y << T::fold(p - q)) + z) << q),
   (p > q) && (p > 0) && (q > 0) && T::opcode_of(x) != static_cast<int>(ir::OpCode::rotate_rows), CIRCUIT_SAVE_FLAG},
  {((y << p) + x) + (z << q), x + (((y << T::fold(p - q)) + z) << q),
   (p > q) && (p > 0) && (q > 0) && T::opcode_of(x) != static_cast<int>(ir::OpCode::rotate_rows), CIRCUIT_SAVE_FLAG},
  {(z << q) + (x + (y << p)), x + (((y << T::fold(p - q)) + z) << q),
   (p > q) && (p > 0) && (q > 0) && T::opcode_of(x) != static_cast<int>(ir::OpCode::rotate_rows), CIRCUIT_SAVE_FLAG},
  {(z << q) + ((y << p) + x), x + (((y << T::fold(p - q)) + z) << q),
   (p > q) && (p > 0) && (q > 0) && T::opcode_of(x) != static_cast<int>(ir::OpCode::rotate_rows), CIRCUIT_SAVE_FLAG},
  {(x + (y << p)) + (z << q), x + (((z << T::fold(p - q)) + y) << q),
   (p < q) && (p > 0) && (q > 0) && T::opcode_of(x) != static_cast<int>(ir::OpCode::rotate_rows), CIRCUIT_SAVE_FLAG},
  {((y << p) + x) + (z << q), x + (((z << T::fold(q - p)) + y) << q),
   (p < q) && (p > 0) && (q > 0) && T::opcode_of(x) != static_cast<int>(ir::OpCode::rotate_rows), CIRCUIT_SAVE_FLAG},
  {(z << q) + (x + (y << p)), x + (((z << T::fold(q - p)) + y) << q),
   (p < q) && (p > 0) && (q > 0) && T::opcode_of(x) != static_cast<int>(ir::OpCode::rotate_rows), CIRCUIT_SAVE_FLAG},
  {(z << q) + ((y << p) + x), x + (((z << T::fold(q - p)) + y) << q),
   (p < q) && (p > 0) && (q > 0) && T::opcode_of(x) != static_cast<int>(ir::OpCode::rotate_rows), CIRCUIT_SAVE_FLAG},
  {(x + (y << p)) + (z << q), x + (((y << T::fold(p - q)) + z) << q),
   (p < q) && (p < 0) && (q < 0) && T::opcode_of(x) != static_cast<int>(ir::OpCode::rotate_rows), CIRCUIT_SAVE_FLAG},
  {((y << p) + x) + (z << q), x + (((y << T::fold(p - q)) + z) << q),
   (p < q) && (p < 0) && (q < 0) && T::opcode_of(x) != static_cast<int>(ir::OpCode::rotate_rows), CIRCUIT_SAVE_FLAG},
  {(z << q) + (x + (y << p)), x + (((y << T::fold(p - q)) + z) << q),
   (p < q) && (p < 0) && (q < 0) && T::opcode_of(x) != static_cast<int>(ir::OpCode::rotate_rows), CIRCUIT_SAVE_FLAG},
  {(z << q) + ((y << p) + x), x + (((y << T::fold(p - q)) + z) << q),
   (p < q) && (p < 0) && (q < 0) && T::opcode_of(x) != static_cast<int>(ir::OpCode::rotate_rows), CIRCUIT_SAVE_FLAG},
  {(x + (y << p)) + (z << p), x + ((y + z) << p), T::opcode_of(x) != static_cast<int>(ir::OpCode::rotate_rows)},
  {((y << p) + x) + (z << p), x + ((y + z) << p), T::opcode_of(x) != static_cast<int>(ir::OpCode::rotate_rows)},
  {(z << p) + (x + (y << p)), x + ((z + y) << p), T::opcode_of(x) != static_cast<int>(ir::OpCode::rotate_rows)},
  {(z << p) + ((y << p) + x), x + ((z + y) << p), T::opcode_of(x) != static_cast<int>(ir::OpCode::rotate_rows)},
  {(x << p) + (y << p), (x + y) << p},
  {(x << p) * (y << p), (x * y) << p},
  {(x << p) - (y << p), (x - y) << p},
  {(x << p) + (y << q), ((x << T::fold(p - q)) + y) << q, (p > q) && (p > 0) && (q > 0), CIRCUIT_SAVE_FLAG},
  {(x << p) + (y << q), ((y << T::fold(q - p)) + x) << p, (q > p) && (p > 0) && (q > 0), CIRCUIT_SAVE_FLAG},
  {(x << p) + (y << q), ((x << T::fold(p - q)) + y) << q, (p < q) && (p > 0) && (q > 0), CIRCUIT_SAVE_FLAG},
  {(x << p) + (y << q), ((y << T::fold(q - p)) + x) << p, (q < p) && (p > 0) && (q > 0), CIRCUIT_SAVE_FLAG},
  {(x << p) + (y << q), ((x << T::fold(p - q)) + y) << q, (p < q) && (p < 0) && (q < 0), CIRCUIT_SAVE_FLAG},
  {(x << p) + (y << q), ((y << T::fold(q - p)) + x) << p, (q < p) && (p < 0) && (q < 0), CIRCUIT_SAVE_FLAG},
  {(x << p) + (y << q), ((x << T::fold(p - q)) + y) << q, (p < q) && (p < 0) && (q < 0), CIRCUIT_SAVE_FLAG},
  {(x << p) + (y << q), ((y << T::fold(q - p)) + x) << p, (q < p) && (p < 0) && (q < 0), CIRCUIT_SAVE_FLAG}};

std::vector<RewriteRule> add_ruleset = {
  {(x + (y << p)) + (z << q), x + (((y << T::fold(p - q)) + z) << q),
   (p > q) && (p > 0) && (q > 0) && T::opcode_of(x) != static_cast<int>(ir::OpCode::rotate_rows), CIRCUIT_SAVE_FLAG},
  {((y << p) + x) + (z << q), x + (((y << T::fold(p - q)) + z) << q),
   (p > q) && (p > 0) && (q > 0) && T::opcode_of(x) != static_cast<int>(ir::OpCode::rotate_rows), CIRCUIT_SAVE_FLAG},
  {(z << q) + (x + (y << p)), x + (((y << T::fold(p - q)) + z) << q),
   (p > q) && (p > 0) && (q > 0) && T::opcode_of(x) != static_cast<int>(ir::OpCode::rotate_rows), CIRCUIT_SAVE_FLAG},
  {(z << q) + ((y << p) + x), x + (((y << T::fold(p - q)) + z) << q),
   (p > q) && (p > 0) && (q > 0) && T::opcode_of(x) != static_cast<int>(ir::OpCode::rotate_rows), CIRCUIT_SAVE_FLAG},
  {(x + (y << p)) + (z << q), x + (((z << T::fold(p - q)) + y) << q),
   (p < q) && (p > 0) && (q > 0) && T::opcode_of(x) != static_cast<int>(ir::OpCode::rotate_rows), CIRCUIT_SAVE_FLAG},
  {((y << p) + x) + (z << q), x + (((z << T::fold(q - p)) + y) << q),
   (p < q) && (p > 0) && (q > 0) && T::opcode_of(x) != static_cast<int>(ir::OpCode::rotate_rows), CIRCUIT_SAVE_FLAG},
  {(z << q) + (x + (y << p)), x + (((z << T::fold(q - p)) + y) << q),
   (p < q) && (p > 0) && (q > 0) && T::opcode_of(x) != static_cast<int>(ir::OpCode::rotate_rows), CIRCUIT_SAVE_FLAG},
  {(z << q) + ((y << p) + x), x + (((z << T::fold(q - p)) + y) << q),
   (p < q) && (p > 0) && (q > 0) && T::opcode_of(x) != static_cast<int>(ir::OpCode::rotate_rows), CIRCUIT_SAVE_FLAG},
  {(x + (y << p)) + (z << q), x + (((y << T::fold(p - q)) + z) << q),
   (p < q) && (p < 0) && (q < 0) && T::opcode_of(x) != static_cast<int>(ir::OpCode::rotate_rows), CIRCUIT_SAVE_FLAG},
  {((y << p) + x) + (z << q), x + (((y << T::fold(p - q)) + z) << q),
   (p < q) && (p < 0) && (q < 0) && T::opcode_of(x) != static_cast<int>(ir::OpCode::rotate_rows), CIRCUIT_SAVE_FLAG},
  {(z << q) + (x + (y << p)), x + (((y << T::fold(p - q)) + z) << q),
   (p < q) && (p < 0) && (q < 0) && T::opcode_of(x) != static_cast<int>(ir::OpCode::rotate_rows), CIRCUIT_SAVE_FLAG},
  {(z << q) + ((y << p) + x), x + (((y << T::fold(p - q)) + z) << q),
   (p < q) && (p < 0) && (q < 0) && T::opcode_of(x) != static_cast<int>(ir::OpCode::rotate_rows), CIRCUIT_SAVE_FLAG},
  {(x + (y << p)) + (z << p), x + ((y + z) << p), T::opcode_of(x) != static_cast<int>(ir::OpCode::rotate_rows)},
  {((y << p) + x) + (z << p), x + ((y + z) << p), T::opcode_of(x) != static_cast<int>(ir::OpCode::rotate_rows)},
  {(z << p) + (x + (y << p)), x + ((z + y) << p), T::opcode_of(x) != static_cast<int>(ir::OpCode::rotate_rows)},
  {(z << p) + ((y << p) + x), x + ((z + y) << p), T::opcode_of(x) != static_cast<int>(ir::OpCode::rotate_rows)},
  {(x << p) + (y << p), (x + y) << p},
  {(x << p) * (y << p), (x * y) << p},
  {(x << p) - (y << p), (x - y) << p},
  {(x << p) + (y << q), ((x << T::fold(p - q)) + y) << q, (p > q) && (p > 0) && (q > 0), CIRCUIT_SAVE_FLAG},
  {(x << p) + (y << q), ((y << T::fold(q - p)) + x) << p, (q > p) && (p > 0) && (q > 0), CIRCUIT_SAVE_FLAG},
  {(x << p) + (y << q), ((x << T::fold(p - q)) + y) << q, (p < q) && (p > 0) && (q > 0), CIRCUIT_SAVE_FLAG},
  {(x << p) + (y << q), ((y << T::fold(q - p)) + x) << p, (q < p) && (p > 0) && (q > 0), CIRCUIT_SAVE_FLAG},
  {(x << p) + (y << q), ((x << T::fold(p - q)) + y) << q, (p < q) && (p < 0) && (q < 0), CIRCUIT_SAVE_FLAG},
  {(x << p) + (y << q), ((y << T::fold(q - p)) + x) << p, (q < p) && (p < 0) && (q < 0), CIRCUIT_SAVE_FLAG},
  {(x << p) + (y << q), ((x << T::fold(p - q)) + y) << q, (p < q) && (p < 0) && (q < 0), CIRCUIT_SAVE_FLAG},
  {(x << p) + (y << q), ((y << T::fold(q - p)) + x) << p, (q < p) && (p < 0) && (q < 0), CIRCUIT_SAVE_FLAG}};

std::vector<RewriteRule> rotations_ruleset = {{(x << p), x, p == 0}, {(x << p) << q, x << T::fold(p + q)}};

/*
(x * 0) => (0) | (x * 1) => (x) : 4
((x + c0) * (x + c1)) => (x * (x + fold(c0 + c1)) + fold(c0 * c1)) : 4
((x + c0) * c1) => (x * c1 + fold(c0 * c1)) : 2
((0 - x) * y) => (0 - x*y) : 2
((x * c0) * c1) => (x * fold(c0 * c1)) : 1
((x * c0) * y) => ((x * y) * c0) : 2
*/

T c0(TermType::constant);
T c1(TermType::constant);

std::vector<RewriteRule> mul_ruleset = {
  {x * c0, c0, T::iszero(c0)} /*rule1*/,
  {c0 * x, c0, T::iszero(c0)} /*rule1*/,
  {x * c0, x, T::isone(c0)} /*rule2*/,
  {c0 * x, x, T::isone(c0)} /*rule2*/,
  {(x + c0) * (x + c1), (x * (x + T::fold(c0 + c1)) + T::fold(c0 * c1))} /*rule3*/,
  {(x + c0) * (c1 + x), (x * (x + T::fold(c0 + c1)) + T::fold(c0 * c1))} /*rule3*/,
  {(c0 + x) * (x + c1), (x * (x + T::fold(c0 + c1)) + T::fold(c0 * c1))} /*rule3*/,
  {(c0 + x) * (c1 + x), (x * (x + T::fold(c0 + c1)) + T::fold(c0 * c1))} /*rule3*/,
  {((x + c0) * c1), (x * c1 + T::fold(c0 * c1))} /*rule4*/,
  {((c0 + x) * c1), (x * c1 + T::fold(c0 * c1))} /*rule4*/,
  {(c1 * (x + c0)), (x * c1 + T::fold(c0 * c1))} /*rule4*/,
  {((c0 + x) * c1), (x * c1 + T::fold(c0 * c1))} /*rule4*/,
  {(c0 - x) * y, c0 - x *y, T::iszero(c0)} /*rule5*/,
  {y * (c0 - x), c0 - x *y, T::iszero(c0)} /*rule5*/,
  {((x * c0) * c1), (x * T::fold(c0 * c1))} /*rule6*/,
  {((c0 * x) * c1), (x * T::fold(c0 * c1))} /*rule6*/,
  {(c1 * (c0 * x)), (x * T::fold(c0 * c1))} /*rule6*/,
  {(c1 * (c0 * x)), (x * T::fold(c0 * c1))} /*rule6*/,
  {((x * c0) * y), ((x * y) * c0)} /*rule7*/,
  {((c0 * x) * y), ((x * y) * c0)} /*rule7*/,
  {(y * (x * c0)), ((x * y) * c0)} /*rule7*/,
  {(y * (c0 * x)), ((x * y) * c0)} /*rule7*/
};

/*
x - 0) => x : 1
(c0 - c1) => (fold(c0 - c1)) : 1
(x - x) => 0 : 1
((x + y) - x) => (y) : 5
((x + c0) - c1) => (x + fold(c0 - c1)) : 9
((x + y) - (x + z)) => (y - z) : 4
((( x + y) + z) - x) => (y + z) : 4
(x - (y + (x - z))) => (z - y) : 4
(x - (y + (x + z))) => (0 - (y + z)) : 4
((x + y) - (z + (w + x))) => (y - (z + w)) : 8
((x - y) - (x + z)) => (0 - y - z) : 2
(((x + y) - z) - x) => (y - z) : 2
(0 - (x + (y - z))) => (z - (x + y)) : 2
(((x - y) - z) - x) => (0 - (y + z)) : 1
Common factor (inv distributive law) : 38, 3
(x*y - z*y) => ((x - z)*y) : 4
((u + x*y) - z*y) => (u + (x - z)*y) : 32
(x*y - x) => (x*(y - 1)) : 4
*/

/*

std::vector<RewriteRule> sub_ruleset = {
  {x - 0, x},
  {x - c0p, x, T::iszero(c0p)},
  {c0p - c1p, T::fold(c0p - c1p), T::isconst(c0p) && T::isconst(c1p)},
  {c0p - c1s, T::fold(c0p - c1s), T::isconst(c0p)},
  {c0s - c1p, T::fold(c0s - c1p), T::isconst(c1p)},
  {c0s - c1s, T::fold(c0s - c1s)},
  {x - x, 0},
  {p1 - p1, 0},
  {(x + y) - x, y},
  {(y + x) - x, y},
  {(p1 + p2) - p1, p2},
  {(p2 + p1) - p1, p2},
  {(x + c0p) - c1p, x + T::fold(c0p - c1p), T::isconst(c0p) && T::isconst(c1p)},
  {(c0p + x) - c1p, x + T::fold(c0p - c1p), T::isconst(c0p) && T::isconst(c1p)},
  {(x + c0p) - c1s, x + T::fold(c0p - c1s), T::isconst(c0p)},
  {(c0p + x) - c1s, x + T::fold(c0p - c1s), T::isconst(c0p)},
  {(x + c0s) - c1p, x + T::fold(c0s - c1p), T::isconst(c1p)},
  {(c0s + x) - c1p, x + T::fold(c0s - c1p), T::isconst(c1p)},
  {(x + c0s) - c1s, x + T::fold(c0s - c1s)},
  {(c0s + x) - c1s, x + T::fold(c0s - c1s)},
  {(x + y) - (x + z), (y - z)},
  {(y + x) - (x + z), (y - z)},
  {(x + y) - (z + x), (y - z)},
  {(y + x) - (z + x), (y - z)},
  {(x + p1) - (x + z), (p1 - z)},
  {(x + p1) - (z + x), (p1 - z)}};

*/

/*
  ((x + y) - (x + z)) => (y - z) : 4
*/

T c2(TermType::constant);

std::vector<RewriteRule> dummy_ruleset3 = {
  {(x + c1) + c2, x + T::fold(c1 + c2)},
  {(x * y) * x, y *square(x)},
  {(x - (y + z)) + z, x - y},
  {(((c1 - x) - y) + c2), (T::fold(c1 + c2) - y) - x},
  {(x + c1) - c2, x + T::fold(c1 - c2)},
  {(x * c1) * c2, x *T::fold(c1 *c2)}};
} // namespace fheco_trs
