#pragma once

#include "fheco/ir/function.hpp"
#include <memory>
#include <ostream>
#include <string>
#include <unordered_map>

namespace fheco::util
{
class ExprPrinter
{
public:
  enum class Mode
  {
    prefix,
    infix,
    infix_explicit_parenthesis,
    posfix
  };

  using TermsStrExpr = std::unordered_map<std::size_t, std::string>;

  ExprPrinter(std::shared_ptr<ir::Function> func, Mode mode = Mode::infix) : func_{move(func)}, mode_{mode}
  {
    update_terms_str_expr(mode);
  }

  void update_terms_str_expr(Mode mode);

  std::string expand_term(std::size_t id, Mode mode, int depth) const;

  void print_outputs_str_expr(std::ostream &os) const;

  void print_terms_str_expr(std::ostream &os) const;

  const std::shared_ptr<ir::Function> func() const { return func_; }

  const TermsStrExpr &terms_str_expr() const { return terms_str_expr_; }

private:
  // for infix representation to reduce unnecessary parenthesis
  static const std::unordered_map<ir::OpCode::Type, int> ops_precedence_;

  std::string expand_term(std::size_t id, Mode mode, int depth, TermsStrExpr &dp) const;

  std::string leaf_str_expr(const ir::Term *term) const;

  std::shared_ptr<ir::Function> func_;

  TermsStrExpr terms_str_expr_{};

  Mode mode_;
};
} // namespace fheco::util

namespace std
{
std::ostream &operator<<(std::ostream &os, const fheco::util::ExprPrinter::TermsStrExpr &terms_str_expr);
} // namespace std
