#include "fheco/ir/term.hpp"
#include "fheco/ir/common.hpp"
#include <utility>

using namespace std;

namespace fheco::ir

{
size_t Term::count_ = 0;

size_t Term::HashParentKey::operator()(const ParentKey &k) const
{
  size_t h = hash<OpCode>()(*k.op_code);
  const auto &operands = *k.operands;
  for (size_t i = 0; i < operands.size(); ++i)
    hash_combine(h, *operands[i]);
  return h;
}

bool Term::EqualParentKey::operator()(const ParentKey &lhs, const ParentKey &rhs) const
{
  if (*lhs.op_code != *rhs.op_code)
    return false;

  const auto &lhs_operands = *lhs.operands;
  const auto &rhs_operands = *rhs.operands;
  if (lhs_operands.size() != rhs_operands.size())
    return false;

  for (size_t i = 0; i < lhs_operands.size(); ++i)
  {
    if (*lhs_operands[i] != *rhs_operands[i])
      return false;
  }

  return true;
}

void Term::replace_in_parents_with(Term *term)
{
  for (auto it = parents_.cbegin(); it != parents_.cend(); ++it)
  {
    auto entry = parents_.extract(it);
    auto parent = entry.mapped();
    for (size_t i = 0; i < parent->operands_.size(); ++i)
    {
      // parent multiplicity can be used to stop once replacement is done, not to traverse the whole operands
      // vector
      if (*parent->operands_[i] == *this)
        parent->operands_[i] = term;
    }
    term->parents_.insert(move(entry));
  }
}
} // namespace fheco::ir
