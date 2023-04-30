#include "fheco/ir/term.hpp"
#include "fheco/ir/common.hpp"

using namespace std;

namespace fheco::ir

{
size_t Term::count_ = 0;

size_t Term::HashPtr::operator()(const Term *p) const
{
  return hash<Term>()(*p);
}

bool Term::EqualPtr::operator()(const Term *lhs, const Term *rhs) const
{
  return *lhs == *rhs;
}

bool Term::ComparePtr::operator()(const Term *lhs, const Term *rhs) const
{
  return *lhs < *rhs;
}
} // namespace fheco::ir
