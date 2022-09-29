#include "fhecompiler.h"

using namespace fhecompiler;

int main() {
    // init(function_name, library_scheme, vector_size)
    init("polynomial", scheme::seal_bfv, 2048);
    // 4x^4 + 8x^3 + 8x^2 + 8x + 4 = 4(x + 1)^2 * (x^2 + 1)
    Ciphertext x("x", var_type::input);
    Scalar shift("shift", var_type::input);
    x.rotate_rows(shift);
    Ciphertext y("y", var_type::output);
    y = 4 * pow(x, 4) + 8 * pow(x, 3) + 8 * square(x) + 8 * x + 4;
    y.rotate_rows(-4);
    y.rotate_columns();
    return 0;
}
