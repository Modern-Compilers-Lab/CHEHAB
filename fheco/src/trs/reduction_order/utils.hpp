#pragma once

#include "matching_term.hpp"
#include <cstddef>
#include <functional>
#include <map>
#include <string>
#include <tuple>

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

std::tuple<int64_t, term_feature_map> fold_he_rotation_steps(const MatchingTerm &term);

std::tuple<int64_t, term_feature_map> fold_raw_data_term(const MatchingTerm &term);

std::vector<std::reference_wrapper<const MatchingTerm>> get_rotation_steps_terms(const MatchingTerm &term);

relation_type term_feature_map_order(const term_feature_map &lhs, const term_feature_map &rhs);

std::tuple<std::string, std::string> create_reduced_exprs(
  term_feature_map lhs_vars_coeff, int64_t lhs_folded_const, term_feature_map rhs_vars_coeff, int64_t rhs_folded_const);

void reduce_feature_maps(term_feature_map &lhs, term_feature_map &rhs);

std::string create_expr(const term_feature_map &vars_coeffs, int64_t folded_const);

bool is_leaf(const MatchingTerm &term);

bool is_ciphertext(const MatchingTerm &term);

bool is_he_mul(const MatchingTerm &term);

bool is_he_square(const MatchingTerm &term);

bool is_he_rotation(const MatchingTerm &term);

bool is_he_add_sub(const MatchingTerm &term);

} // namespace fheco_trs
