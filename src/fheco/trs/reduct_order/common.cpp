#include "fheco/trs/ops_overloads.hpp"
#include "fheco/trs/reduct_order/common.hpp"

using namespace std;

namespace fheco::trs
{
int sum_cipher_leaves_xdepths(const TermMatcher &term)
{
  int result = 0;
  sum_cipher_leaves_xdepths_util(term, 0, result);
  return result;
}

void sum_cipher_leaves_xdepths_util(const TermMatcher &term, int init_xdepth, int &result)
{
  if (term.is_leaf())
  {
    if (is_cipher(term))
      result += init_xdepth;
    return;
  }

  if (is_mul(term) || is_square(term) || is_mul_plain(term))
    ++init_xdepth;
  for (const auto &operand : term.operands())
    sum_cipher_leaves_xdepths_util(operand, init_xdepth, result);
}

int sum_cipher_leaves_depths(const TermMatcher &term)
{
  int result = 0;
  sum_cipher_leaves_depths_util(term, 0, result);
  return result;
}

void sum_cipher_leaves_depths_util(const TermMatcher &term, int init_depth, int &result)
{
  if (term.is_leaf())
  {
    if (is_cipher(term))
      result += init_depth;
    return;
  }

  if (is_cipher(term))
    ++init_depth;
  for (const auto &operand : term.operands())
    sum_cipher_leaves_depths_util(operand, init_depth, result);
}

TermsFeatureValues compute_cipher_vars_xdepths(const TermMatcher &term)
{
  if (term.is_leaf())
  {
    if (is_cipher(term))
      return {{*term.label(), 0}};

    return {{}};
  }

  TermsFeatureValues result;
  auto is_mul_op = is_mul(term);
  auto is_square_op = is_square(term);
  for (auto const &operand : term.operands())
  {
    for (const auto &e : compute_cipher_vars_xdepths(operand))
    {
      auto xdepth = e.second;
      if (is_mul_op || is_square_op)
        ++xdepth;
      auto it = result.find(e.first);
      if (it == result.end())
        result.emplace(e.first, xdepth);
      else
      {
        if (xdepth > it->second)
          it->second = xdepth;
      }
    }
  }
  return result;
}

TermsFeatureValues compute_cipher_vars_depths(const TermMatcher &term)
{
  if (term.is_leaf())
  {
    if (is_cipher(term))
      return {{*term.label(), 0}};

    return {{}};
  }

  TermsFeatureValues result;
  auto is_cipher_op = is_cipher(term);
  for (auto const &operand : term.operands())
  {
    for (const auto &e : compute_cipher_vars_depths(operand))
    {
      auto depth = e.second;
      if (is_cipher_op)
        ++depth;
      auto it = result.find(e.first);
      if (it == result.end())
        result.emplace(e.first, depth);
      else
      {
        if (depth > it->second)
          it->second = depth;
      }
    }
  }
  return result;
}

int sum_leaves_depths(const TermMatcher &term)
{
  int result = 0;
  sum_leaves_depths_util(term, 0, result);
  return result;
}

void sum_leaves_depths_util(const TermMatcher &term, int init_depth, int &result)
{
  if (term.is_leaf())
  {
    result += init_depth;
    return;
  }

  ++init_depth;
  for (const auto &operand : term.operands())
    sum_leaves_depths_util(operand, init_depth, result);
}

TermsFeatureValues compute_vars_depths(const TermMatcher &term)
{
  if (term.is_leaf())
  {
    return {{*term.label(), 0}};

    return {{}};
  }

  TermsFeatureValues result;
  for (auto const &operand : term.operands())
  {
    for (const auto &e : compute_vars_depths(operand))
    {
      auto depth = e.second;
      ++depth;
      auto it = result.find(e.first);
      if (it == result.end())
        result.emplace(e.first, depth);
      else
      {
        if (depth > it->second)
          it->second = depth;
      }
    }
  }
  return result;
}

TermsFeatureValues count_class_vars_occ(const TermMatcher &term, const TermClassChecker &class_checker)
{
  TermsFeatureValues result;
  count_class_vars_occ_util(term, class_checker, result);
  return result;
}

void count_class_vars_occ_util(
  const TermMatcher &term, const TermClassChecker &class_checker, TermsFeatureValues &result)
{
  if (term.is_variable())
  {
    if (class_checker(term))
    {
      auto label = *term.label();
      auto it = result.find(label);
      if (it == result.end())
        result.emplace(label, 1);
      else
        ++it->second;
    }
    return;
  }

  for (const auto &operand : term.operands())
    count_class_vars_occ_util(operand, class_checker, result);
}

int count_class_subterms(const TermMatcher &term, const TermClassChecker &class_checker)
{
  int result = 0;
  count_class_subterms_util(term, class_checker, result);
  return result;
}

void count_class_subterms_util(const TermMatcher &term, const TermClassChecker &class_checker, int &result)
{
  if (class_checker(term))
    ++result;

  for (const auto &operand : term.operands())
    count_class_subterms_util(operand, class_checker, result);
}

string make_phi_str(
  const TermMatcher &term, const TermClassChecker &class_a_checker, const TermClassChecker &class_b_checker)
{
  string result{};
  make_phi_str_util(term, class_a_checker, class_b_checker, result);
  return result;
}

void make_phi_str_util(
  const TermMatcher &term, const TermClassChecker &class_a_checker, const TermClassChecker &class_b_checker,
  string &result)
{
  if (class_a_checker(term))
    result += "a";
  else if (class_b_checker(term))
    result += "b";
  else
    result += "-";

  for (const auto &operand : term.operands())
    make_phi_str_util(operand, class_a_checker, class_b_checker, result);
}

OpGenMatcher sum_rotation_steps(const TermMatcher &term)
{
  OpGenMatcher result{};
  sum_rotation_steps_util(term, result);
  return result;
}

void sum_rotation_steps_util(const TermMatcher &term, OpGenMatcher &result)
{
  if (is_rotate(term))
  {
    if (result.id())
      result += term.op_code().steps();
    else
      result = term.op_code().steps();
  }

  for (const auto &operand : term.operands())
    sum_rotation_steps_util(operand, result);
}

CompResult terms_feature_values_order(const TermsFeatureValues &lhs, const TermsFeatureValues &rhs)
{
  bool eq = true;
  for (const auto &e : lhs)
  {
    auto it = rhs.find(e.first);
    if (it != rhs.end())
    {
      if (e.second < it->second)
        return CompResult::less;

      if (e.second > it->second)
        eq = false;
    }
    else
      eq = false;
  }
  return eq ? CompResult::equal : CompResult::greater;
}

ostream &operator<<(ostream &os, const TermsFeatureValues &terms_feature_values)
{
  for (const auto &e : terms_feature_values)
    os << e.first << ": " << e.second << '\n';
  return os;
}

bool is_cipher(const TermMatcher &term)
{
  return term.type() == TermMatcherType::cipher || term.type() == TermMatcherType::term;
}

bool is_plain(const TermMatcher &term)
{
  return term.type() == TermMatcherType::plain;
}

bool is_var(const TermMatcher &term)
{
  return term.is_variable() && !is_const(term);
}

bool is_const(const TermMatcher &term)
{
  return term.type() == TermMatcherType::const_;
}

bool is_plain_or_const(const TermMatcher &term)
{
  return term.type() == TermMatcherType::plain || term.type() == TermMatcherType::const_;
}

bool is_leaf(const TermMatcher &term)
{
  return term.is_leaf();
}

bool is_mul(const TermMatcher &term)
{
  return term.op_code().type() == ir::OpCode::Type::mul && is_cipher(term.operands()[0]) &&
         is_cipher(term.operands()[1]);
}

bool is_square(const TermMatcher &term)
{
  return is_cipher(term) && term.op_code().type() == ir::OpCode::Type::square;
}

bool is_rotate(const TermMatcher &term)
{
  return is_cipher(term) && term.op_code().type() == ir::OpCode::Type::rotate;
}

bool is_mul_plain(const TermMatcher &term)
{
  return is_cipher(term) && term.op_code().type() == ir::OpCode::Type::mul &&
         (is_plain_or_const(term.operands()[0]) || is_plain_or_const(term.operands()[1]));
}

bool is_he_add(const TermMatcher &term)
{
  return is_cipher(term) &&
         (term.op_code().type() == ir::OpCode::Type::add || term.op_code().type() == ir::OpCode::Type::sub ||
          term.op_code().type() == ir::OpCode::Type::negate);
}

bool is_cipher_cipher_op(const TermMatcher &term)
{
  bool cipher_operands = true;
  for (const auto &operand : term.operands())
  {
    if (!is_cipher(operand))
    {
      cipher_operands = false;
      break;
    }
  }
  return !term.is_leaf() && cipher_operands;
}

bool is_plain_plain_op(const TermMatcher &term)
{
  return is_plain(term) && !term.is_leaf();
}

bool is_const_op(const TermMatcher &term)
{
  bool const_operands = true;
  for (const auto &operand : term.operands())
  {
    if (!is_const(operand))
    {
      const_operands = false;
      break;
    }
  }
  return !term.is_leaf() && const_operands;
}
} // namespace fheco::trs
