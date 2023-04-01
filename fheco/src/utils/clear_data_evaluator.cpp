#include "clear_data_evaluator.hpp"

namespace std
{
bool operator==(const vector<uint64_t> &lhs, const vector<int64_t> &rhs)
{
  if (lhs.size() != rhs.size())
    return false;

  for (std::size_t i = 0; i < lhs.size(); ++i)
  {
    if (lhs[i] != rhs[i])
      return false;
  }

  return true;
}
} // namespace std
