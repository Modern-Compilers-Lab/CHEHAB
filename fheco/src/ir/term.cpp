#include "term.hpp"
#include <cstddef>
#include <utility>

using namespace std;

namespace fhecompiler
{
namespace ir
{
  Term::Term(string label, OpCode op_code, vector<Term *> operands)
    : label_{move(label)}, op_code_{move(op_code)}, operands_{move(operands)}, type_{OpCode::deduce_result_type(
                                                                                 op_code_, operands_)}
  {}

  void Term::replace_with(Term *t)
  {
    for (auto it = parents_.begin(); it != parents_.end();)
    {
      auto parent = *it;
      for (size_t i = 0; i < parent->operands_.size(); ++i)
      {
        if (*parent->operands_[i] == *this)
        {
          parent->operands_[i] = t;
          continue;
        }
      }
      t->parents_.insert(parent);
      it = parents_.erase(it);
    }
  }
} // namespace ir
} // namespace fhecompiler
