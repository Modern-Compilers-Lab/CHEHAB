#pragma once

#include "matching_term.hpp"
#include <cstddef>
#include <functional>
#include <map>
#include <string>

namespace fheco_trs
{

enum class relation_type
{
  eq,
  lt,
  gt
};

using term_feature_map = std::unordered_map<std::string, int>;

std::string make_key(const MatchingTerm &term);

int64_t get_term_value(const MatchingTerm &term);

term_feature_map cipher_vars_xdepths(const MatchingTerm &term);

std::size_t sum_cipher_leaves_xdepth(const MatchingTerm &term);

term_feature_map count_leaves_class_occ(
  const MatchingTerm &term, std::function<bool(const MatchingTerm &)> node_checker);

std::size_t count_nodes_class(const MatchingTerm &term, std::function<bool(const MatchingTerm &)> node_checker);

int64_t fold_he_rotation_steps(const MatchingTerm &term, term_feature_map &vars_coeffs);

relation_type term_feature_map_order(const term_feature_map &lhs, const term_feature_map &rhs);

void reduce_feature_map_order(term_feature_map &lhs, term_feature_map &rhs);

bool is_leaf(const MatchingTerm &term);

bool is_ciphertext(const MatchingTerm &term);

bool is_he_mul(const MatchingTerm &term);

bool is_he_square(const MatchingTerm &term);

bool is_he_rotation(const MatchingTerm &term);

bool is_he_add_sub(const MatchingTerm &term);

} // namespace fheco_trs
