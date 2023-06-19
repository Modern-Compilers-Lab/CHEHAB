#pragma once

#include "fheco/trs/term_matcher.hpp"
#include <cstddef>
#include <functional>
#include <ostream>
#include <string>
#include <unordered_map>

namespace fheco::trs
{
enum class CompResult
{
  less,
  not_generalizable,
  equal,
  greater
};

using TermsFeatureValues = std::unordered_map<std::string, int>;

using TermClassChecker = std::function<bool(const TermMatcher &)>;

int sum_cipher_leaves_xdepths(const TermMatcher &term);

void sum_cipher_leaves_xdepths_util(const TermMatcher &term, int init_xdepth, int &result);

int sum_cipher_leaves_depths(const TermMatcher &term);

void sum_cipher_leaves_depths_util(const TermMatcher &term, int init_depth, int &result);

TermsFeatureValues compute_cipher_vars_xdepths(const TermMatcher &term);

TermsFeatureValues compute_cipher_vars_depths(const TermMatcher &term);

int sum_leaves_depths(const TermMatcher &term);

void sum_leaves_depths_util(const TermMatcher &term, int init_depth, int &result);

TermsFeatureValues compute_vars_depths(const TermMatcher &term);

TermsFeatureValues count_class_vars_occ(const TermMatcher &term, const TermClassChecker &class_checker);

void count_class_vars_occ_util(
  const TermMatcher &term, const TermClassChecker &class_checker, TermsFeatureValues &result);

int count_class_subterms(const TermMatcher &term, const TermClassChecker &class_checker);

void count_class_subterms_util(const TermMatcher &term, const TermClassChecker &class_checker, int &result);

std::string make_phi_str(
  const TermMatcher &term, const TermClassChecker &class_a_checker, const TermClassChecker &class_b_checker);

void make_phi_str_util(
  const TermMatcher &term, const TermClassChecker &class_a_checker, const TermClassChecker &class_b_checker,
  std::string &result);

OpGenMatcher sum_rotation_steps(const TermMatcher &term);

void sum_rotation_steps_util(const TermMatcher &term, OpGenMatcher &result);

CompResult terms_feature_values_order(const TermsFeatureValues &lhs, const TermsFeatureValues &rhs);

bool is_cipher(const TermMatcher &term);

bool is_plain(const TermMatcher &term);

bool is_var(const TermMatcher &term);

bool is_const(const TermMatcher &term);

bool is_plain_or_const(const TermMatcher &term);

bool is_leaf(const TermMatcher &term);

bool is_mul(const TermMatcher &term);

bool is_square(const TermMatcher &term);

bool is_rotate(const TermMatcher &term);

bool is_mul_plain(const TermMatcher &term);

bool is_he_add(const TermMatcher &term);

bool is_cipher_cipher_op(const TermMatcher &term);

bool is_plain_plain_op(const TermMatcher &term);

bool is_const_op(const TermMatcher &term);

std::ostream &operator<<(std::ostream &os, const TermsFeatureValues &terms_feature_values);
} // namespace fheco::trs
