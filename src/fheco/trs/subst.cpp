#include "fheco/ir/term.hpp"
#include "fheco/trs/subst.hpp"
#include <stdexcept>
#include <utility>

using namespace std;

namespace fheco::trs
{

bool Subst::insert(TermMatcher term_matcher, ir::Term *term)
{
  if (!term_matcher.is_variable())
    throw invalid_argument("term_matcher must be a varible");

  auto [it, inserted] = terms_matching_.emplace(move(term_matcher), term);
  return inserted || it->second->id() == term->id();
}

bool Subst::insert(OpGenMatcher op_gen_matcher, int val)
{
  if (!op_gen_matcher.is_variable())
    throw invalid_argument("op_gen_matcher must be a varible");

  auto [it, inserted] = op_gen_matching_.emplace(move(op_gen_matcher), val);
  return inserted || it->second == val;
}

ir::Term *Subst::get(const TermMatcher &term_matcher) const
{
  if (auto it = terms_matching_.find(term_matcher); it != terms_matching_.end())
    return it->second;

  throw invalid_argument("substitution for term_matcher not provided");
}

int Subst::get(const OpGenMatcher &op_gen_matcher) const
{
  if (auto it = op_gen_matching_.find(op_gen_matcher); it != op_gen_matching_.end())
    return it->second;

  throw invalid_argument("substitution for op_gen_matcher not provided");
}

ostream &operator<<(ostream &os, const Subst &subst)
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
