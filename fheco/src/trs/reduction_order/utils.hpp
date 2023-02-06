#pragma once

#include "matching_term.hpp"
#include <cstddef>
#include <functional>
#include <map>

namespace fheco_trs
{

enum class relation_type
{
  eq,
  lt,
  gt
};

using term_feature_map = std::unordered_map<std::size_t, int>;

term_feature_map cipher_vars_xdepths(const MatchingTerm &term, int &result_term_xdepth);

void sum_cipher_leaves_xdepth(const MatchingTerm &term, std::size_t init_xdepth, std::size_t &result);

void count_leaves_class_occ(
  const MatchingTerm &term, std::function<bool(const MatchingTerm &)> node_checker, term_feature_map &result);

void count_nodes_class(
  const MatchingTerm &term, std::function<bool(const MatchingTerm &)> node_checker, std::size_t &result);

void fold_he_rotation_steps(const MatchingTerm &term, term_feature_map &vars_coeffs, int &folded_const);

relation_type term_feature_map_order(const term_feature_map &lhs, const term_feature_map &rhs);

bool is_ciphertext(const MatchingTerm &term);

bool is_he_mul(const MatchingTerm &term);

bool is_he_square(const MatchingTerm &term);

bool is_he_rotation(const MatchingTerm &term);

bool is_he_add_sub(const MatchingTerm &term);

} // namespace fheco_trs
