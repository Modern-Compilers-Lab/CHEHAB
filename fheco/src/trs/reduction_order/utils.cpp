#include "utils.hpp"
#include <stdexcept>

using namespace std;

namespace fheco_trs
{
std::string create_key(const MatchingTerm &term)
{
  return term.get_label().has_value() ? *term.get_label() : "#" + to_string(term.get_term_id());
}

term_feature_map cipher_vars_xdepths(const MatchingTerm &term)
{
  if (is_leaf(term))
  {
    if (is_ciphertext(term))
      return {{create_key(term), 0}};

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
  function<void(const MatchingTerm &, size_t, size_t &)> core_sum_cipher_leaves_xdepth =
    [&core_sum_cipher_leaves_xdepth](const MatchingTerm &term, size_t init_xdepth, size_t &result) -> void {
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
      core_sum_cipher_leaves_xdepth(operand, xdepth, result);
  };

  size_t result = 0;
  core_sum_cipher_leaves_xdepth(term, 0, result);
  return result;
}

term_feature_map count_leaves_class_occ(const MatchingTerm &term, function<bool(const MatchingTerm &)> node_checker)
{
  function<void(const MatchingTerm &, function<bool(const MatchingTerm &)>, term_feature_map &)>
    core_count_leaves_class_occ =
      [&core_count_leaves_class_occ](
        const MatchingTerm &term, function<bool(const MatchingTerm &)> node_checker, term_feature_map &result) -> void {
    if (is_leaf(term))
    {
      if (node_checker(term))
      {
        string key = create_key(term);
        auto it = result.find(key);
        if (it == result.end())
          result.insert({key, 1});
        else
          ++it->second;
      }
      return;
    }

    for (const MatchingTerm &operand : term.get_operands())
      core_count_leaves_class_occ(operand, node_checker, result);
  };

  term_feature_map result;
  core_count_leaves_class_occ(term, node_checker, result);
  return result;
}

size_t count_nodes_class(const MatchingTerm &term, function<bool(const MatchingTerm &)> node_checker)
{
  function<void(const MatchingTerm &, function<bool(const MatchingTerm &)>, size_t &)> core_count_nodes_class =
    [&core_count_nodes_class](
      const MatchingTerm &term, function<bool(const MatchingTerm &)> node_checker, size_t &result) -> void {
    if (node_checker(term))
      ++result;

    for (const MatchingTerm &operand : term.get_operands())
      core_count_nodes_class(operand, node_checker, result);
  };

  size_t result = 0;
  core_count_nodes_class(term, node_checker, result);
  return result;
}

int fold_he_rotation_steps(const MatchingTerm &term, term_feature_map &vars_coeffs)
{
  function<term_feature_map(const MatchingTerm &, int &)> process_step_term =
    [&process_step_term](const MatchingTerm &term, int &folded_const) -> term_feature_map {
    if (term.get_term_type() != TermType::rawDataType)
      throw logic_error("invalid rotation step");

    if (is_leaf(term))
    {
      if (term.get_value().has_value())
        folded_const += get<int64_t>(get<ir::ScalarValue>(*term.get_value()));
      else
        return {{create_key(term), 1}};
    }

    OpCode opcode = term.get_opcode();
    term_feature_map vars_coeffs;
    int operand_idx = 0;
    for (const MatchingTerm &operand : term.get_operands())
    {
      for (const auto &e : process_step_term(operand, folded_const))
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
    return vars_coeffs;
  };

  function<void(const MatchingTerm &, term_feature_map &, int &)> core_fold_he_rotation_steps =
    [&process_step_term,
     &core_fold_he_rotation_steps](const MatchingTerm &term, term_feature_map &vars_coeffs, int &folded_const) -> void {
    bool he_rot = is_he_rotation(term);
    for (const MatchingTerm &operand : term.get_operands())
    {
      if (he_rot && operand.get_term_type() == TermType::rawDataType)
      {
        term_feature_map operand_result = process_step_term(operand, folded_const);
        for (const auto &e : operand_result)
        {
          auto it = vars_coeffs.find(e.first);
          if (it == vars_coeffs.end())
            vars_coeffs.insert(e);
          else
            it->second += e.second;
        }
      }
      else
        core_fold_he_rotation_steps(operand, vars_coeffs, folded_const);
    }
  };

  int folded_const = 0;
  core_fold_he_rotation_steps(term, vars_coeffs, folded_const);
  return folded_const;
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
