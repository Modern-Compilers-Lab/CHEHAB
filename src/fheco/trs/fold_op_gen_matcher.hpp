#pragma once

#include "fheco/trs/op_gen_matcher.hpp"
#include "fheco/trs/substitution.hpp"
#include <cstddef>
#include <ostream>
#include <unordered_map>
#include <vector>

namespace fheco::trs
{
using OpGenMatcherBlocksCoeffs =
  std::unordered_map<OpGenMatcher::RefWrapp, int, OpGenMatcher::HashRefWrapp, OpGenMatcher::EqualrRefWrapp>;

int fold_op_gen_matcher(const OpGenMatcher &op_gen_matcher, const Substitution &subst);

void operate(const OpGenOpCode &op_code, const std::vector<int> &args, int &dest);

void operate_unary(const OpGenOpCode &op_code, int arg, int &dest);

void operate_binary(const OpGenOpCode &op_code, int arg1, int arg2, int &dest);

OpGenMatcher fold_symbolic_op_gen_matcher(const OpGenMatcher &op_gen_matcher);

OpGenMatcherBlocksCoeffs compute_blocks_coeffs(const OpGenMatcher &op_gen_matcher);

void compute_blocks_coeffs_util(
  const OpGenMatcher &op_gen_matcher, bool pos_neg, OpGenMatcherBlocksCoeffs &blocks_coeffs);

std::ostream &operator<<(std::ostream &os, const OpGenMatcherBlocksCoeffs &blocks_coeffs);
} // namespace fheco::trs
