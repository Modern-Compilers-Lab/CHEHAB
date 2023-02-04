#include "component_orders.hpp"
#include "utils.hpp"

using namespace std;

namespace fheco_trs
{

relation_type xdepth_order(const MatchingTerm &lhs, const MatchingTerm &rhs)
{
  size_t lhs_xdepth = 0;
  size_t rhs_xdepth = 0;
  term_feature_map lhs_cipher_vars_xdepth = cipher_vars_xdepths(lhs, lhs_xdepth);
  term_feature_map rhs_cipher_vars_xdepth = cipher_vars_xdepths(rhs, rhs_xdepth);
  if (!term_feature_map_ge(lhs_cipher_vars_xdepth, rhs_cipher_vars_xdepth))
    return relation_type::lt;

  int diff = 1;
  int modified_rhs_xdepth = rhs_xdepth - diff;
  if ((int)lhs_xdepth > modified_rhs_xdepth)
    return relation_type::gt;
  if ((int)lhs_xdepth < modified_rhs_xdepth)
    return relation_type::lt;
  return relation_type::eq;
}

relation_type he_op_class_order(
  const MatchingTerm &lhs, const MatchingTerm &rhs, function<bool(const MatchingTerm &)> op_class_checker)
{
  term_feature_map lhs_cipher_vars_occ;
  count_leaves_class_occ(lhs, &is_ciphertext, lhs_cipher_vars_occ);
  term_feature_map rhs_cipher_vars_occ;
  count_leaves_class_occ(rhs, &is_ciphertext, rhs_cipher_vars_occ);
  if (!term_feature_map_ge(lhs_cipher_vars_occ, rhs_cipher_vars_occ))
    return relation_type::lt;

  size_t he_mul_lhs = 0;
  count_nodes_class(lhs, op_class_checker, he_mul_lhs);
  size_t he_mul_rhs = 0;
  count_nodes_class(rhs, op_class_checker, he_mul_rhs);
  if (he_mul_lhs > he_mul_rhs)
    return relation_type::gt;
  if (he_mul_lhs < he_mul_rhs)
    return relation_type::lt;
  return relation_type::eq;
}

relation_type he_rotation_steps_order(const MatchingTerm &lhs, const MatchingTerm &rhs)
{
  term_feature_map lhs_cipher_vars_occ;
  count_leaves_class_occ(lhs, &is_ciphertext, lhs_cipher_vars_occ);
  term_feature_map rhs_cipher_vars_occ;
  count_leaves_class_occ(rhs, &is_ciphertext, rhs_cipher_vars_occ);
  if (!term_feature_map_ge(lhs_cipher_vars_occ, rhs_cipher_vars_occ))
    return relation_type::lt;

  set<size_t> lhs_rotation_steps;
  set<size_t> rhs_rotation_steps;
  count_he_rotation_steps(lhs, lhs_rotation_steps);
  count_he_rotation_steps(rhs, rhs_rotation_steps);

  size_t lhs_steps_count = lhs_rotation_steps.size();
  int diff = 1;
  int modified_rhs_steps_count = rhs_rotation_steps.size() - diff;
  if ((int)lhs_steps_count > modified_rhs_steps_count)
    return relation_type::gt;
  if ((int)lhs_steps_count < modified_rhs_steps_count)
    return relation_type::lt;
  return relation_type::eq;
}
} // namespace fheco_trs
