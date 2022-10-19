#include "fhecompiler.h"

using namespace fhecompiler;

int main() {
    // init(function_name, library_scheme, vector_size)
    init("polynomial_fct0", scheme::seal_bfv, 2048);
    // 4x^4 + 8x^3 + 8x^2 + 8x + 4 = 4(x + 1)^2 * (x^2 + 1)
    Ciphertext x("x", var_type::input);
    Scalar shift("shift", var_type::input);
    x.rotate(shift);
    Ciphertext z = 8 * exponentiate(x, 2);
    Ciphertext y("y", var_type::output);
    y = 4 * exponentiate(x, 4) + z * x + z + 8 * x + 4;
    y.rotate(-4);
    codegen("generated_defs_seal_bfv_example.hpp");
    return 0;
}
