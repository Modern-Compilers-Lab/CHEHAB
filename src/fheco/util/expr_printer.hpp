#pragma once

#include "fheco/ir/op_code.hpp"
#include "fheco/trs/op_gen_matcher.hpp"
#include "fheco/trs/rule.hpp"
#include "fheco/trs/term_matcher.hpp"
#include <cstddef>
#include <memory>
#include <ostream>
#include <string>
#include <unordered_map>

namespace fheco::ir
{
class Func;

class Term;
} // namespace fheco::ir

namespace fheco::util
{
class ExprPrinter
{
public:
  enum class Mode
  {
    none,
    prefix,
    infix,
    infix_expl_paren,
    posfix
  };

  using TermsStrExpr = std::unordered_map<std::size_t, std::string>;

  static std::string make_rule_str_repr(
    const trs::Rule &rule, bool show_label = false, Mode mode = Mode::infix_expl_paren);

  static std::string make_term_matcher_str_expr(const trs::TermMatcher &term_matcher, Mode mode = Mode::infix);

  static std::string make_op_gen_matcher_str_expr(const trs::OpGenMatcher &op_gen_matcher, Mode mode = Mode::infix);

  ExprPrinter(std::shared_ptr<ir::Func> func) : func_{std::move(func)}, mode_{Mode::none} {}

  void make_terms_str_expr(Mode mode = Mode::infix);

  void print_expand_outputs_str_expr(std::ostream &os, int depth = 3, Mode mode = Mode::infix_expl_paren) const;

  std::string expand_term_str_expr(const ir::Term *term, int depth = 3, Mode mode = Mode::infix_expl_paren) const;

  std::string make_leaf_str_expr(const ir::Term *term) const;

  void print_outputs_str_expr(std::ostream &os) const;

  void print_terms_str_expr(std::ostream &os) const;

  const std::shared_ptr<ir::Func> func() const { return func_; }

  const TermsStrExpr &terms_str_exprs() const { return terms_str_exprs_; }

  Mode mode() const { return mode_; }

private:
  // for infix representation to reduce unnecessary parenthesis
  static const std::unordered_map<ir::OpCode::Type, int> ops_precedence_;

  static const std::unordered_map<trs::OpGenOpCode::Type, int> op_gen_matcher_ops_precedence_;

  std::shared_ptr<ir::Func> func_;

  TermsStrExpr terms_str_exprs_{};

  Mode mode_;
};

std::ostream &operator<<(std::ostream &os, const ExprPrinter::TermsStrExpr &terms_str_expr);
} // namespace fheco::util
