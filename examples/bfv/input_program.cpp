#include "fhecompiler.h"

int main()
{
  fhecompiler::init("example0", scheme::bfv);
  fhecompiler::Ciphertext a("a", fhecompiler::var_type::input);
  fhecompiler::Ciphertext b("b", fhecompiler::var_type::input);
  fhecompiler::Ciphertext r("r", fhecompiler::var_type::output);
  r = a + b;
  fhecompiler::codegen("generated_defs_seal_bfv_example.hpp");
  return 0;
}
