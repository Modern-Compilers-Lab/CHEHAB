#pragma once

#include "matching_term.hpp"
#include <cstddef>
#include <functional>
#include <map>
#include <set>

namespace fheco_trs
{

using term_feature_map = std::unordered_map<std::size_t, std::size_t>;

term_feature_map cipher_vars_xdepths(const MatchingTerm &term, std::size_t &result_xdepth);

void count_leaves_class_occ(
  const MatchingTerm &term, std::function<bool(const MatchingTerm &)> node_checker, term_feature_map &result);

void count_nodes_class(
  const MatchingTerm &term, std::function<bool(const MatchingTerm &)> node_checker, std::size_t &result);

void count_he_rotation_steps(const MatchingTerm &term, std::set<std::size_t> &result);

bool term_feature_map_ge(const term_feature_map &lhs, const term_feature_map &rhs);

bool is_ciphertext(const MatchingTerm &term);

bool is_he_mul(const MatchingTerm &term);

bool is_he_square(const MatchingTerm &term);

bool is_he_rotation(const MatchingTerm &term);

bool is_he_add_sub(const MatchingTerm &term);
} // namespace fheco_trs
