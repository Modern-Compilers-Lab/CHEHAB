#pragma once

#include "fheco/trs/rule.hpp"
#include "fheco/trs/term_matcher.hpp"
#include <memory>
#include <ostream>

namespace fheco::ir
{
class Func;
class Term;
} // namespace fheco::ir

namespace fheco::util
{
void draw_ir(
  const std::shared_ptr<ir::Func> &func, std::ostream &os, bool id_as_label = false, bool show_key = false,
  bool impose_operands_order = false);

void draw_term(
  const std::shared_ptr<ir::Func> &func, const ir::Term *term, int depth, std::ostream &os, bool id_as_label = false,
  bool show_key = false, bool impose_operands_order = false);

void draw_rule(const trs::Rule &rule, std::ostream &os, bool show_key = false, bool impose_operands_order = false);

void draw_term_matcher(
  const trs::TermMatcher &term_matcher, std::ostream &os, bool show_key = false, bool impose_operands_order = false);
} // namespace fheco::util
