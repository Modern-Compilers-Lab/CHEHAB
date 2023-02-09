#include "utils.hpp"
#include <functional>
#include <set>
#include <stdexcept>

using namespace std;

namespace fheco_trs
{
std::string make_key(const MatchingTerm &term)
{
  return term.get_label().has_value() ? *term.get_label() : "#" + to_string(term.get_term_id());
}

int64_t get_term_value(const MatchingTerm &term)
{
  return get<int64_t>(get<ir::ScalarValue>(*term.get_value()));
}

term_feature_map cipher_vars_xdepths(const MatchingTerm &term)
{
  if (is_leaf(term))
  {
    if (is_ciphertext(term))
      return {{make_key(term), 0}};

    return {{}};
  }

  term_feature_map result;
  bool he_mul = is_he_mul(term);
  bool he_square = is_he_square(term);
  for (const MatchingTerm &operand : term.get_operands())
  {
    for (const auto &e : cipher_vars_xdepths(operand))
    {
      string key = e.first;
      int xdepth = e.second;
      if (he_mul || he_square)
        ++xdepth;
      auto it = result.find(e.first);
      if (it == result.end())
        result.insert({key, xdepth});
      else
      {
        if (xdepth > it->second)
          it->second = xdepth;
      }
    }
  }
  return result;
}

size_t sum_cipher_leaves_xdepth(const MatchingTerm &term)
{
  function<void(const MatchingTerm &, size_t, size_t &)> kernel_sum_cipher_leaves_xdepth =
    [&kernel_sum_cipher_leaves_xdepth](const MatchingTerm &term, size_t init_xdepth, size_t &result) -> void {
    if (is_leaf(term))
    {
      if (is_ciphertext(term))
        result += init_xdepth;
      return;
    }

    size_t xdepth = init_xdepth;
    if (is_he_mul(term) || is_he_square(term))
      ++xdepth;
    for (const MatchingTerm &operand : term.get_operands())
      kernel_sum_cipher_leaves_xdepth(operand, xdepth, result);
  };

  size_t result = 0;
  kernel_sum_cipher_leaves_xdepth(term, 0, result);
  return result;
}

term_feature_map count_leaves_class_occ(const MatchingTerm &term, function<bool(const MatchingTerm &)> node_checker)
{
  function<void(const MatchingTerm &, function<bool(const MatchingTerm &)>, term_feature_map &)>
    kernel_count_leaves_class_occ =
      [&kernel_count_leaves_class_occ](
        const MatchingTerm &term, function<bool(const MatchingTerm &)> node_checker, term_feature_map &result) -> void {
    if (is_leaf(term))
    {
      if (node_checker(term))
      {
        string key = make_key(term);
        auto it = result.find(key);
        if (it == result.end())
          result.insert({key, 1});
        else
          ++it->second;
      }
      return;
    }

    for (const MatchingTerm &operand : term.get_operands())
      kernel_count_leaves_class_occ(operand, node_checker, result);
  };

  term_feature_map result;
  kernel_count_leaves_class_occ(term, node_checker, result);
  return result;
}

size_t count_nodes_class(const MatchingTerm &term, function<bool(const MatchingTerm &)> node_checker)
{
  function<void(const MatchingTerm &, function<bool(const MatchingTerm &)>, size_t &)> kernel_count_nodes_class =
    [&kernel_count_nodes_class](
      const MatchingTerm &term, function<bool(const MatchingTerm &)> node_checker, size_t &result) -> void {
    if (node_checker(term))
      ++result;

    for (const MatchingTerm &operand : term.get_operands())
      kernel_count_nodes_class(operand, node_checker, result);
  };

  size_t result = 0;
  kernel_count_nodes_class(term, node_checker, result);
  return result;
}

tuple<int64_t, term_feature_map> fold_he_rotation_steps(const MatchingTerm &term)
{
  function<void(const MatchingTerm &, term_feature_map &, int64_t &)> kernel_fold_he_rotation_steps =
    [&kernel_fold_he_rotation_steps](
      const MatchingTerm &term, term_feature_map &vars_coeffs, int64_t &folded_const) -> void {
    bool he_rot = is_he_rotation(term);
    for (const MatchingTerm &operand : term.get_operands())
    {
      if (he_rot && operand.get_term_type() == TermType::rawDataType)
      {
        auto [operand_folded_const, operand_vars_coeffs] = fold_raw_data_term(operand);
        folded_const += operand_folded_const;
        for (const auto &e : operand_vars_coeffs)
        {
          auto it = vars_coeffs.find(e.first);
          if (it == vars_coeffs.end())
            vars_coeffs.insert(e);
          else
            it->second += e.second;
        }
      }
      else
        kernel_fold_he_rotation_steps(operand, vars_coeffs, folded_const);
    }
  };

  int64_t folded_const = 0;
  term_feature_map vars_coeffs;
  kernel_fold_he_rotation_steps(term, vars_coeffs, folded_const);
  return {folded_const, vars_coeffs};
}

tuple<int64_t, term_feature_map> fold_raw_data_term(const MatchingTerm &term)
{
  function<term_feature_map(const MatchingTerm &, int64_t &, int64_t)> kernel_fold_raw_data_term =
    [&kernel_fold_raw_data_term](const MatchingTerm &term, int64_t &folded_const, int64_t coeff) -> term_feature_map {
    if (term.get_term_type() != TermType::rawDataType)
      throw logic_error("invalid rotation step");

    if (is_leaf(term))
    {
      if (term.get_value().has_value())
        folded_const += get_term_value(term) * coeff;
      else
        return {{make_key(term), coeff}};
    }

    OpCode opcode = term.get_opcode();
    const vector<MatchingTerm> &operands = term.get_operands();
    term_feature_map vars_coeffs;
    if (opcode == OpCode::mul)
    {
      int raw_coeff_idx = 0;
      while (raw_coeff_idx < 2)
      {
        if (is_leaf(operands[raw_coeff_idx]) && operands[raw_coeff_idx].get_value().has_value())
          break;
        ++raw_coeff_idx;
      }
      if (raw_coeff_idx < 2)
      {
        term_feature_map operand_vars_coeffs = kernel_fold_raw_data_term(
          operands[(raw_coeff_idx + 1) % 2], folded_const, coeff * get_term_value(operands[raw_coeff_idx]));
        for (const auto &e : operand_vars_coeffs)
        {
          auto it = vars_coeffs.find(e.first);
          if (it == vars_coeffs.end())
            vars_coeffs.insert(e);
          else
            it->second += e.second;
        }
      }
      else
        throw logic_error("unsupported rotation step term");
    }
    else
    {
      int operand_idx = 0;
      for (const MatchingTerm &operand : operands)
      {
        for (const auto &e : kernel_fold_raw_data_term(operand, folded_const, coeff))
        {
          auto it = vars_coeffs.find(e.first);
          if (opcode == OpCode::add)
          {
            if (it == vars_coeffs.end())
              vars_coeffs.insert(e);
            else
              it->second += e.second;
          }
          else if (opcode == OpCode::sub)
          {
            if (operand_idx == 0)
            {
              if (it == vars_coeffs.end())
                vars_coeffs.insert(e);
              else
                it->second += e.second;
            }
            else
            {
              if (it == vars_coeffs.end())
                vars_coeffs.insert({e.first, -e.second});
              else
                it->second -= e.second;
            }
          }
          else if (opcode == OpCode::negate)
          {
            if (it == vars_coeffs.end())
              vars_coeffs.insert({e.first, -e.second});
            else
              it->second -= e.second;
          }
          else
            throw logic_error("unsupported rotation step term");
        }
        ++operand_idx;
      }
    }
    return vars_coeffs;
  };

  int64_t folded_const = 0;
  term_feature_map vars_coeffs = kernel_fold_raw_data_term(term, folded_const, 1);
  return {folded_const, vars_coeffs};
}

vector<reference_wrapper<const MatchingTerm>> get_rotation_steps_terms(const MatchingTerm &term)
{
  function<void(const MatchingTerm &, vector<reference_wrapper<const MatchingTerm>> &, set<string> &)>
    kernel_get_rotation_steps_terms = [&kernel_get_rotation_steps_terms](
                                        const MatchingTerm &term,
                                        vector<reference_wrapper<const MatchingTerm>> &steps_terms,
                                        set<string> &terms_keys) -> void {
    bool he_rot = is_he_rotation(term);
    for (const MatchingTerm &operand : term.get_operands())
    {
      if (he_rot && operand.get_term_type() == TermType::rawDataType)
      {
        string key = make_key(operand);
        auto it = terms_keys.find(key);
        if (it == terms_keys.end())
        {
          terms_keys.insert(key);
          steps_terms.push_back(ref(operand));
        }
      }
      else
        kernel_get_rotation_steps_terms(operand, steps_terms, terms_keys);
    }
  };

  vector<reference_wrapper<const MatchingTerm>> steps_terms;
  set<string> terms_keys;
  kernel_get_rotation_steps_terms(term, steps_terms, terms_keys);
  return steps_terms;
}

relation_type term_feature_map_order(const term_feature_map &lhs, const term_feature_map &rhs)
{
  bool eq = true;
  for (const auto &e : lhs)
  {
    auto it = rhs.find(e.first);
    if (it != rhs.end())
    {
      if (e.second < it->second)
        return relation_type::lt;

      if (e.second > it->second)
        eq = false;
    }
  }
  return eq ? relation_type::eq : relation_type::gt;
}

tuple<string, string> create_reduced_exprs(
  term_feature_map lhs_vars_coeff, int64_t lhs_folded_const, term_feature_map rhs_vars_coeff, int64_t rhs_folded_const)
{
  reduce_feature_maps(lhs_vars_coeff, rhs_vars_coeff);
  string lhs_sum_expr, rhs_sum_expr;
  if (lhs_folded_const > rhs_folded_const)
  {
    lhs_sum_expr = create_expr(lhs_vars_coeff, lhs_folded_const - rhs_folded_const);
    rhs_sum_expr = create_expr(rhs_vars_coeff, 0);
  }
  else
  {
    lhs_sum_expr = create_expr(lhs_vars_coeff, 0);
    rhs_sum_expr = create_expr(rhs_vars_coeff, rhs_folded_const - lhs_folded_const);
  }
  return {lhs_sum_expr, rhs_sum_expr};
}

void reduce_feature_maps(term_feature_map &lhs, term_feature_map &rhs)
{
  for (auto &e : lhs)
  {
    string var_key = e.first;
    int lhs_coeff = e.second;
    auto it = rhs.find(var_key);
    if (it == rhs.end())
      rhs.insert({var_key, 0});
    int rhs_coeff = rhs[var_key];
    if (lhs_coeff > rhs_coeff)
    {
      e.second -= rhs_coeff;
      rhs[var_key] -= rhs_coeff;
    }
    else
    {
      e.second -= lhs_coeff;
      rhs[var_key] -= lhs_coeff;
    }
  }
}

string create_expr(const term_feature_map &vars_coeffs, int64_t folded_const)
{
  string expr;
  string plus_sign = " + ";
  for (const auto &e : vars_coeffs)
  {
    auto create_var_expr = [&plus_sign](const string &var_key, int coeff) -> string {
      if (coeff == 0)
        return "";

      if (coeff == 1)
        return var_key + plus_sign;

      return to_string(coeff) + "*" + var_key + plus_sign;
    };

    expr += create_var_expr(e.first, e.second);
  }
  if (folded_const != 0)
    expr += to_string(folded_const);
  else
  {
    if (expr.length() != 0)
      expr.erase(expr.length() - plus_sign.length());
    else
      expr += "0";
  }
  return expr;
}

bool is_leaf(const MatchingTerm &term)
{
  return term.get_operands().size() == 0;
}

bool is_he_mul(const MatchingTerm &term)
{
  if ((term.get_opcode() == OpCode::mul || term.get_opcode() == OpCode::mul_plain) && is_ciphertext(term))
    return true;
  return false;
}

bool is_he_square(const MatchingTerm &term)
{
  if (term.get_opcode() == OpCode::square && is_ciphertext(term))
    return true;
  return false;
}

bool is_he_rotation(const MatchingTerm &term)
{
  if (
    (term.get_opcode() == OpCode::rotate || term.get_opcode() == OpCode::rotate_rows ||
     term.get_opcode() == OpCode::rotate_columns) &&
    is_ciphertext(term))
    return true;
  return false;
}

bool is_he_add_sub(const MatchingTerm &term)
{
  if (
    (term.get_opcode() == OpCode::add || term.get_opcode() == OpCode::add_plain || term.get_opcode() == OpCode::sub ||
     term.get_opcode() == OpCode::sub_plain) &&
    is_ciphertext(term))
    return true;
  return false;
}

bool is_ciphertext(const MatchingTerm &term)
{
  return term.get_term_type() == TermType::ciphertextType;
}

} // namespace fheco_trs
