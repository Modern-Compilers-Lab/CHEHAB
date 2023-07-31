#pragma once

#include "fheco/trs/op_gen_matcher.hpp"
#include "fheco/trs/rule.hpp"
#include "fheco/trs/term_matcher.hpp"
#include <memory>
#include <ostream>
#include <string>

namespace fheco::ir
{
class Func;
class Term;
} // namespace fheco::ir

namespace fheco::util
{
void draw_ir(
  const std::shared_ptr<ir::Func> &func, std::ostream &os, bool show_key = false, bool impose_operands_order = false,
  bool id_as_label = false);

void draw_term(
  const std::shared_ptr<ir::Func> &func, const ir::Term *term, int depth, std::ostream &os, bool show_key = false,
  bool impose_operands_order = false, bool id_as_label = false);

void draw_rule(const trs::Rule &rule, std::ostream &os, bool show_key = false, bool impose_operands_order = false);

void draw_term_matcher(
  const trs::TermMatcher &term_matcher, std::ostream &os, bool show_key = false, bool impose_operands_order = false);

std::string make_term_attrs(const std::shared_ptr<ir::Func> &func, const ir::Term *term, bool id_as_label);

std::string make_term_label(const std::shared_ptr<ir::Func> &func, const ir::Term *term, bool id_as_label);

void draw_term_matcher_util(
  const trs::TermMatcher &term_matcher, std::ostream &os, bool impose_operands_order, int &leaf_occ_id);

void draw_op_gen_matcher_util(
  const trs::OpGenMatcher &op_gen_matcher, std::ostream &os, bool impose_operands_order, int &leaf_occ_id);
} // namespace fheco::util
