#include "fheco/ir/func.hpp"
#include "fheco/ir/term.hpp"
#include "fheco/trs/substitution.hpp"
#include "fheco/util/expr_printer.hpp"
#include <stdexcept>
#include <utility>

using namespace std;

namespace fheco::trs
{

bool Substitution::insert(TermMatcher term_matcher, ir::Term *term)
{
  if (!term_matcher.is_variable())
    throw invalid_argument("term_matcher must be a varible");

  auto [it, inserted] = terms_matching_.emplace(move(term_matcher), term);
  return inserted || it->second->id() == term->id();
}

bool Substitution::insert(OpGenMatcher op_gen_matcher, int val)
{
  if (!op_gen_matcher.is_variable())
    throw invalid_argument("op_gen_matcher must be a varible");

  auto [it, inserted] = op_gen_matching_.emplace(move(op_gen_matcher), val);
  return inserted || it->second == val;
}

ir::Term *Substitution::get(const TermMatcher &term_matcher) const
{
  if (auto it = terms_matching_.find(term_matcher); it != terms_matching_.end())
    return it->second;

  throw invalid_argument("substitution for term_matcher not provided");
}

int Substitution::get(const OpGenMatcher &op_gen_matcher) const
{
  if (auto it = op_gen_matching_.find(op_gen_matcher); it != op_gen_matching_.end())
    return it->second;

  throw invalid_argument("substitution for op_gen_matcher not provided");
}

void pprint_substitution(const shared_ptr<ir::Func> &func, const Substitution &subst, ostream &os)
{
  util::ExprPrinter expr_printer{func};
  os << "{";
  const auto &terms_matching = subst.terms_matching();
  for (auto it = terms_matching.cbegin(); it != terms_matching.cend();)
  {
    const auto &v = *it;
    os << *v.first.label() << ": " << expr_printer.expand_term_str_expr(v.second);
    ++it;
    if (it != terms_matching.cend())
      os << ", ";
    else
      break;
  }

  const auto &op_gen_matching = subst.op_gen_matching();
  if (op_gen_matching.size())
    os << ", ";
  for (auto it = op_gen_matching.cbegin(); it != op_gen_matching.cend();)
  {
    const auto &v = *it;
    os << *v.first.label() << ": " << util::ExprPrinter::make_op_gen_matcher_str_expr(v.second);
    ++it;
    if (it != op_gen_matching.cend())
      os << ", ";
    else
      break;
  }
  os << "}";
}

ostream &operator<<(ostream &os, const Substitution &subst)
{
  os << "Terms substitution:\n";
  for (const auto &v : subst.terms_matching())
    os << *v.first.label() << ": $" << v.second->id() << '\n';
  os << "Operation generators substitution:\n";
  for (const auto &v : subst.op_gen_matching())
    os << *v.first.label() << ": " << v.second << '\n';
  return os;
}
} // namespace fheco::trs
