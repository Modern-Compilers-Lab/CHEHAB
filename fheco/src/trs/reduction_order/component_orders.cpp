#include "component_orders.hpp"
#include <iostream>
#include <string>

using namespace std;

namespace fheco_trs
{
relation_type xdepth_order(const MatchingTerm &lhs, const MatchingTerm &rhs)
{
  term_feature_map lhs_cipher_vars_xdepth = cipher_vars_xdepths(lhs);
  term_feature_map rhs_cipher_vars_xdepth = cipher_vars_xdepths(rhs);
  if (term_feature_map_order(lhs_cipher_vars_xdepth, rhs_cipher_vars_xdepth) == relation_type::lt)
    return relation_type::lt;

  term_feature_map lhs_cipher_vars_occ = count_leaves_class_occ(lhs, &is_ciphertext);
  term_feature_map rhs_cipher_vars_occ = count_leaves_class_occ(rhs, &is_ciphertext);
  if (term_feature_map_order(lhs_cipher_vars_occ, rhs_cipher_vars_occ) == relation_type::lt)
    return relation_type::lt;

  size_t lhs_sum_leaves_xdepth = sum_cipher_leaves_xdepth(lhs);
  size_t rhs_sum_leaves_xdepth = sum_cipher_leaves_xdepth(rhs);
  if (lhs_sum_leaves_xdepth > rhs_sum_leaves_xdepth)
    return relation_type::gt;
  if (lhs_sum_leaves_xdepth < rhs_sum_leaves_xdepth)
    return relation_type::lt;
  return relation_type::eq;
}

relation_type he_op_class_order(
  const MatchingTerm &lhs, const MatchingTerm &rhs, function<bool(const MatchingTerm &)> op_class_checker)
{
  term_feature_map lhs_cipher_vars_occ = count_leaves_class_occ(lhs, &is_ciphertext);
  term_feature_map rhs_cipher_vars_occ = count_leaves_class_occ(rhs, &is_ciphertext);
  if (term_feature_map_order(lhs_cipher_vars_occ, rhs_cipher_vars_occ) == relation_type::lt)
    return relation_type::lt;

  size_t he_mul_lhs = count_nodes_class(lhs, op_class_checker);
  size_t he_mul_rhs = count_nodes_class(rhs, op_class_checker);
  if (he_mul_lhs > he_mul_rhs)
    return relation_type::gt;
  if (he_mul_lhs < he_mul_rhs)
    return relation_type::lt;
  return relation_type::eq;
}

relation_type he_rotation_steps_order(const MatchingTerm &lhs, const MatchingTerm &rhs)
{
  cout << "rotation steps domain (0 <= step < nb_slots) predicates:" << endl;
  MatchingTerm tmp = lhs || rhs;
  auto steps_terms = get_rotation_steps_terms(tmp);
  for (const MatchingTerm &e : steps_terms)
  {
    auto [step_folded_const, step_vars_occ] = fold_raw_data_term(e);
    auto cond1 = create_reduced_exprs(step_vars_occ, step_folded_const, {}, 0);
    auto cond2 = create_reduced_exprs(step_vars_occ, step_folded_const, {{"nb_slots", 1}}, 0);
    cout << get<0>(cond1) << " >= " << get<1>(cond1) << " && " << get<0>(cond2) << " < " << get<1>(cond2) << endl;
  }

  term_feature_map lhs_cipher_vars_occ = count_leaves_class_occ(lhs, &is_ciphertext);
  term_feature_map rhs_cipher_vars_occ = count_leaves_class_occ(rhs, &is_ciphertext);
  if (term_feature_map_order(lhs_cipher_vars_occ, rhs_cipher_vars_occ) == relation_type::lt)
    return relation_type::lt;

  auto [lhs_folded_const, lhs_steps_vars_coeffs] = fold_he_rotation_steps(lhs);
  auto [rhs_folded_const, rhs_steps_vars_coeffs] = fold_he_rotation_steps(rhs);
  relation_type coeffs_rel = term_feature_map_order(lhs_steps_vars_coeffs, rhs_steps_vars_coeffs);

  if (
    (coeffs_rel == relation_type::eq && lhs_folded_const > rhs_folded_const) ||
    (coeffs_rel == relation_type::gt && lhs_folded_const >= rhs_folded_const))
    return relation_type::gt;

  if (coeffs_rel == relation_type::lt || lhs_folded_const < rhs_folded_const)
  {
    auto [lhs_sum_expr, rhs_sum_expr] =
      create_reduced_exprs(lhs_steps_vars_coeffs, lhs_folded_const, rhs_steps_vars_coeffs, rhs_folded_const);
    cout << "rotation steps sum predicate: " << lhs_sum_expr << " > " << rhs_sum_expr << endl;
    return relation_type::lt;
  }

  return relation_type::eq;
}

} // namespace fheco_trs
