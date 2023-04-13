#include "term.hpp"
#include <cstddef>

using namespace std;

namespace fhecompiler
{
namespace ir
{
  void Term::replace_with(Term *t)
  {
    for (auto it = parents_.cbegin(); it != parents_.cend();)
    {
      auto &[label, parent] = *it;
      for (size_t i = 0; i < parent->operands_.size(); ++i)
      {
        if (*parent->operands_[i] == *this)
        {
          parent->operands_[i] = t;
          continue;
        }
      }
      t->add_parent(parent);
      it = parents_.erase(it);
    }
  }
} // namespace ir
} // namespace fhecompiler
