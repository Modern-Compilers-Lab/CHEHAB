#include "utils.hpp"
#include <cstdint>

using namespace std;

namespace fheco_trs
{

term_feature_map cipher_vars_xdepths(const MatchingTerm &term, size_t &result_term_xdepth)
{
  if (term.get_opcode() == OpCode::undefined)
  {
    if (is_ciphertext(term))
      return {{term.get_term_id(), 0}};

    return {{}};
  }

  term_feature_map result;
  bool he_mul = is_he_mul(term);
  bool he_square = is_he_square(term);
  for (const MatchingTerm &operand : term.get_operands())
  {
    for (const auto &e : cipher_vars_xdepths(operand, result_term_xdepth))
    {
      size_t id = e.first;
      size_t xdepth = e.second;
      if (he_mul || he_square)
        ++xdepth;

      if (xdepth > result_term_xdepth)
        result_term_xdepth = xdepth;

      auto it = result.find(e.first);
      if (it == result.end())
        result.insert({id, xdepth});
      else
      {
        if (xdepth > it->second)
          it->second = xdepth;
      }
    }
  }
  return result;
}

void count_leaves_class_occ(
  const MatchingTerm &term, function<bool(const MatchingTerm &)> class_checker, term_feature_map &result)
{
  if (term.get_opcode() == OpCode::undefined)
  {
    if (class_checker(term))
    {
      size_t id = term.get_term_id();
      auto it = result.find(id);
      if (it == result.end())
        result.insert({id, 1});
      else
        ++it->second;
    }
    return;
  }

  for (const MatchingTerm &operand : term.get_operands())
    count_leaves_class_occ(operand, class_checker, result);
}

void count_nodes_class(const MatchingTerm &term, function<bool(const MatchingTerm &)> class_checker, size_t &result)
{
  if (class_checker(term))
    ++result;

  for (const MatchingTerm &operand : term.get_operands())
  {
    count_nodes_class(operand, class_checker, result);
  }
}

void count_he_rotation_steps(const MatchingTerm &term, set<size_t> &result)
{
  bool he_rot = is_he_rotation(term);
  for (const MatchingTerm &operand : term.get_operands())
  {
    if (he_rot && operand.get_term_type() == TermType::rawDataType)
      result.insert(get<uint64_t>(get<ir::ScalarValue>(*operand.get_value())));
    else
      count_he_rotation_steps(operand, result);
  }
}

bool term_feature_map_ge(const term_feature_map &lhs, const term_feature_map &rhs)
{
  for (const auto &e : lhs)
  {
    auto it = rhs.find(e.first);
    if (it != rhs.end())
    {
      if (e.second < it->second)
        return false;
    }
  }
  return true;
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
