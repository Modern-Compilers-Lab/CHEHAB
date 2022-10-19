#include <iomanip>
#include <map>
#include <string>
#include <variant>
#include <vector>

#include "seal/seal.h"

using namespace std;
using namespace seal;

// BFV scheme allows modular arithmetic
using Scalar = uint64_t;
using ClearInputs = map<string, variant<vector<Scalar>, Scalar>>;
using ClearOutputs = ClearInputs;
using EncodedInputs = map<string, variant<Ciphertext, Plaintext, Scalar>>;
using EncodedOutputs = EncodedInputs;

void polynomial_fct0(const SEALContext &context, const RelinKeys &relin_keys,
                     const GaloisKeys &galois_keys, EncodedInputs &inputs,
                     EncodedOutputs &outputs) {
    Ciphertext x = get<Ciphertext>(inputs["x"]);
    Scalar shift = get<Scalar>(inputs["shift"]);
    Evaluator evaluator(context);
    // 4x^4 + 8x^3 + 8x^2 + 8x + 4 = 4(x + 1)^2 * (x^2 + 1)
    /*
        Rotation in our DSL is about 1D vectors.
        The compiler will translate the rotation on a 1D vector that the user
       operates on into rotations on 2*(n/2) seal matrix
    */
    // evaluator.rotate_rows_inplace(x, shift, galois_keys);
    Ciphertext x1 = x;
    Plaintext plain1("1");
    evaluator.add_plain_inplace(x1, plain1);
    evaluator.square_inplace(x1);
    evaluator.relinearize_inplace(x1, relin_keys);
    evaluator.mod_switch_to_next_inplace(x1);
    evaluator.multiply_plain_inplace(x1, Plaintext("4"));
    Ciphertext x2 = x;
    evaluator.square_inplace(x2);
    evaluator.relinearize_inplace(x2, relin_keys);
    evaluator.mod_switch_to_next_inplace(x2);
    evaluator.add_plain_inplace(x2, plain1);
    Ciphertext y = x1;
    evaluator.multiply_inplace(y, x2);
    evaluator.relinearize_inplace(y, relin_keys);
    // evaluator.rotate_rows_inplace(y, -4, galois_keys);
    outputs["y"] = y;
}

void polynomial_fct0_inputs_setup(const SEALContext &context,
                                  const PublicKey &public_key,
                                  ClearInputs &clear_inputs,
                                  EncodedInputs &encoded_inputs) {
    Encryptor encryptor(context, public_key);
    BatchEncoder batch_encoder(context);
    vector<Scalar> &x_clear = get<vector<Scalar>>(clear_inputs["x"]);
    Scalar shift = get<Scalar>(clear_inputs["shift"]);
    Plaintext x_plain;
    batch_encoder.encode(x_clear, x_plain);
    Ciphertext x_cipher;
    encryptor.encrypt(x_plain, x_cipher);
    encoded_inputs["x"] = x_cipher;
    encoded_inputs["shift"] = shift;
}

void polynomial_fct0_ouputs_setup(const SEALContext &context,
                                  const SecretKey &secret_key,
                                  EncodedOutputs &encoded_outputs,
                                  ClearOutputs &clear_outputs) {
    Decryptor decryptor(context, secret_key);
    BatchEncoder batch_encoder(context);
    Ciphertext &y_cipher = get<Ciphertext>(encoded_outputs["y"]);
    Plaintext y_plain;
    decryptor.decrypt(y_cipher, y_plain);
    vector<Scalar> y_clear;
    batch_encoder.decode(y_plain, y_clear);
    clear_outputs["y"] = y_clear;
}

/*
Helper function: Prints a matrix of values.
*/
template <typename T>
inline void print_matrix(std::vector<T> matrix, std::size_t row_size) {
    /*
    We're not going to print every column of the matrix (there are 2048).
    Instead print this many slots from beginning and end of the matrix.
    */
    std::size_t print_size = 5;

    std::cout << std::endl;
    std::cout << "    [";
    for (std::size_t i = 0; i < print_size; i++) {
        std::cout << std::setw(3) << std::right << matrix[i] << ",";
    }
    std::cout << std::setw(3) << " ...,";
    for (std::size_t i = row_size - print_size; i < row_size; i++) {
        std::cout << std::setw(3) << matrix[i]
                  << ((i != row_size - 1) ? "," : " ]\n");
    }
    std::cout << "    [";
    for (std::size_t i = row_size; i < row_size + print_size; i++) {
        std::cout << std::setw(3) << matrix[i] << ",";
    }
    std::cout << std::setw(3) << " ...,";
    for (std::size_t i = 2 * row_size - print_size; i < 2 * row_size; i++) {
        std::cout << std::setw(3) << matrix[i]
                  << ((i != 2 * row_size - 1) ? "," : " ]\n");
    }
    std::cout << std::endl;
}
