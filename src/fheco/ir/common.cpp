#include "fheco/ir/common.hpp"
#include <stdexcept>

using namespace std;

namespace fheco::ir
{
size_t HashConstVal::operator()(const ConstVal &value_var) const
{
  size_t size = slot_count_;
  return visit(
    ir::overloaded{
      [size](const PackedVal &packed_val) {
        size_t h = 0;
        for (auto it = packed_val.cbegin(); it != packed_val.cend(); ++it)
          hash_combine(h, *it);
        return h;
      },
      [size](ScalarVal scalar_val) {
        return hash<integer>{}(scalar_val);
      }},
    value_var);
}

string term_type_str_repr(TermType term_type)
{
  switch (term_type)
  {
  case TermType::ciphertext:
    return "ctxt";
    break;

  case TermType::plaintext:
    return "ptxt";
    break;

  case TermType::scalar:
    return "scalar";
    break;

  default:
    throw invalid_argument("invalid term_type");
    break;
  }
}
} // namespace fheco::ir
