#pragma once

#include "fheco/trs/op_gen_matcher.hpp"
#include "fheco/trs/subst.hpp"
#include <cstddef>
#include <functional>
#include <ostream>
#include <unordered_map>
#include <vector>

namespace fheco::trs
{
struct HashOpGenMatcherRef
{
  std::size_t operator()(const std::reference_wrapper<const OpGenMatcher> &matcher_ref) const;
};

struct EqualOpGenMatcherRef
{
  bool operator()(
    const std::reference_wrapper<const OpGenMatcher> &lhs, const std::reference_wrapper<const OpGenMatcher> &rhs) const;
};

using OpGenMatcherBlocksCoeffs =
  std::unordered_map<std::reference_wrapper<const OpGenMatcher>, int, HashOpGenMatcherRef, EqualOpGenMatcherRef>;

int fold_op_gen_matcher(const OpGenMatcher &op_gen_matcher, const Subst &subst);

void operate(const OpGenOpCode &op_code, const std::vector<int> &args, int &dest);

void operate_unary(const OpGenOpCode &op_code, int arg, int &dest);

void operate_binary(const OpGenOpCode &op_code, int arg1, int arg2, int &dest);

OpGenMatcher fold_symbolic_op_gen_matcher(const OpGenMatcher &op_gen_matcher);

OpGenMatcherBlocksCoeffs compute_blocks_coeffs(const OpGenMatcher &op_gen_matcher);

void compute_blocks_coeffs_util(
  const OpGenMatcher &op_gen_matcher, bool pos_neg, OpGenMatcherBlocksCoeffs &blocks_coeffs);

std::ostream &operator<<(std::ostream &os, const OpGenMatcherBlocksCoeffs &blocks_coeffs);
} // namespace fheco::trs
