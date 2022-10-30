#include "fhecompiler.h"

using namespace fhecompiler;

int main() {
    init("example0", scheme::bfv, 2048);
    Ciphertext a("a", var_type::input);
    Ciphertext b("b", var_type::input);
    Ciphertext r("r", var_type::output);
    r = a + b;
    codegen("generated_defs_seal_bfv_example.hpp");
    return 0;
}
