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
  cout << "rotation steps domain: 0 <= step < nb_slots" << endl;

  term_feature_map lhs_cipher_vars_occ = count_leaves_class_occ(lhs, &is_ciphertext);
  term_feature_map rhs_cipher_vars_occ = count_leaves_class_occ(rhs, &is_ciphertext);
  if (term_feature_map_order(lhs_cipher_vars_occ, rhs_cipher_vars_occ) == relation_type::lt)
    return relation_type::lt;

  term_feature_map lhs_steps_vars_coeffs;
  int lhs_folded_const = fold_he_rotation_steps(lhs, lhs_steps_vars_coeffs);
  term_feature_map rhs_steps_vars_coeffs;
  int rhs_folded_const = fold_he_rotation_steps(rhs, rhs_steps_vars_coeffs);
  relation_type coeffs_rel = term_feature_map_order(lhs_steps_vars_coeffs, rhs_steps_vars_coeffs);

  if (
    (coeffs_rel == relation_type::eq && lhs_folded_const > rhs_folded_const) ||
    (coeffs_rel == relation_type::gt && lhs_folded_const >= rhs_folded_const))
    return relation_type::gt;

  if (coeffs_rel == relation_type::lt || lhs_folded_const < rhs_folded_const)
  {
    reduce_feature_map_order(lhs_steps_vars_coeffs, rhs_steps_vars_coeffs);
    string lhs_sum_expr, rhs_sum_expr;
    string plus_sign = " + ";
    for (const auto &e : lhs_steps_vars_coeffs)
    {
      auto create_var_expr = [&plus_sign](const string &var_id, int coeff) -> string {
        if (coeff == 0)
          return "";

        if (coeff == 1)
          return var_id + plus_sign;

        return to_string(coeff) + "*" + var_id + plus_sign;
      };
      string var_id = e.first;
      lhs_sum_expr += create_var_expr(var_id, lhs_steps_vars_coeffs[var_id]);
      rhs_sum_expr += create_var_expr(var_id, rhs_steps_vars_coeffs[var_id]);
    }
    auto finalize_expr_without_folded_const = [&plus_sign](string &expr) -> void {
      if (expr.length() != 0)
        expr.erase(expr.length() - plus_sign.length());
      else
        expr += "0";
    };

    if (lhs_folded_const > rhs_folded_const)
    {
      lhs_sum_expr += to_string(lhs_folded_const - rhs_folded_const);
      finalize_expr_without_folded_const(rhs_sum_expr);
    }
    else if (lhs_folded_const < rhs_folded_const)
    {
      rhs_sum_expr += to_string(rhs_folded_const - lhs_folded_const);
      finalize_expr_without_folded_const(lhs_sum_expr);
    }
    else
    {
      finalize_expr_without_folded_const(lhs_sum_expr);
      finalize_expr_without_folded_const(rhs_sum_expr);
    }
    cout << "rotation steps sum predicate: " << lhs_sum_expr << " > " << rhs_sum_expr << " ?" << endl;

    return relation_type::lt;
  }

  return relation_type::eq;
}

} // namespace fheco_trs
