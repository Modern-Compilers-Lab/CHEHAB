#include "term_type.hpp"

namespace fhecompiler
{
namespace ir
{
  int TermType::count_ = 0;

  // order of definition is important for type deduction (OpCode::deduce_result_type)
  const TermType TermType::ciphertext = TermType("ctxt");
  const TermType TermType::plaintext = TermType("ptxt");
  const TermType TermType::scalar = TermType("scalar");
} // namespace ir
} // namespace fhecompiler
