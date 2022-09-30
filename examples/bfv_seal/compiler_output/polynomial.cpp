#include <map>
#include <string>

#include "seal/seal.h"
#include "some_def.h"
/*
Scalar class
EncodedInputsMap = map<string, variant<Ciphertext, Plaintext, Scalar>>
EncodedOutputsMap = map<string, variant<Ciphertext, Plaintext, Scalar>>
*/

using namespace std;
using namespace seal;

EncodedOutputsMap polynomial(const SEALContext &context,
                             const RelinKeys &relin_keys,
                             const GaloisKeys &galois_keys,
                             const EncodedInputsMap &inputs) {
    Ciphertext &x = inputs["x"];
    Scalar &shift = inputs["shift"];
    Evaluator evaluator(context);
    // 4x^4 + 8x^3 + 8x^2 + 8x + 4 = 4(x + 1)^2 * (x^2 + 1)
    // handle 1d vector rotation
    evaluator.rotate_rows_inplace(x, shift.as_int(), galois_keys);
    Ciphertext &x1 = x;
    Plaintext plain1("1");
    evaluator.add_plain_inplace(x1, plain1);
    evaluator.square_inplace(x1);
    evaluator.relinearize_inplace(x1, relin_keys);
    evaluator.mod_switch_to_next_inplace(x1);
    evaluator.multiply_plain_inplace(x1, Plaintext("4"));
    Ciphertext &x2 = x;
    evaluator.square_inplace(x2);
    evaluator.relinearize_inplace(x2, relin_keys);
    evaluator.mod_switch_to_next_inplace(x2);
    evaluator.add_plain_inplace(x2, plain1);
    Plaintext y = x1;
    evaluator.multiply_inplace(y, x2);
    evaluator.relinearize_inplace(y, relin_keys);
    /*
        Rotation in our DSL is about 1D vectors. 
        The compiler will translate the rotation on a 1D vector that the user operates on into rotations on 2*(n/2) seal matrix
    */ 
    //evaluator.rotate_rows_inplace(y, -4, galois_keys);
    return {{"y", y}};
}
