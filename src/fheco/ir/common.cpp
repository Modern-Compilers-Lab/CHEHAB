#include "fheco/ir/common.hpp"
#include <stdexcept>

using namespace std;

namespace fheco::ir
{
size_t HashPackedVal::operator()(const PackedVal &packed_val) const
{
  size_t h = 0;
  for (auto it = packed_val.cbegin(); it != packed_val.cend(); ++it)
    hash_combine(h, *it);
  return h;
}

string term_type_str_repr(TermType term_type)
{
  switch (term_type)
  {
  case TermType::cipher:
    return "ctxt";

  case TermType::plain:
    return "ptxt";

  default:
    throw invalid_argument("invalid term_type");
  }
}
} // namespace fheco::ir
