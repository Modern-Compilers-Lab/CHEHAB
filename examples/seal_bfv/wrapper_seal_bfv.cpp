#include <cmath>
#include <fstream>

#include "generated_defs_seal_bfv_example.hpp"

uint64_t scalar_polynomial_fct0(uint64_t x) {
    return 4 * pow(x, 4) + 8 * pow(x, 3) + 8 * pow(x, 2) + 8 * x + 4;
}

int main() {
    // 4x^4 + 8x^3 + 8x^2 + 8x + 4 = 4(x + 1)^2 * (x^2 + 1)

    // Load encryption parameters
    EncryptionParameters parms;
    ifstream parameters_ifs("encryption_parameters", ios::binary);
    parms.load(parameters_ifs);
    parameters_ifs.close();
    // Create a context using loaded parameters
    SEALContext context(parms);
    // Load the secret key
    SecretKey secret_key;
    ifstream secret_key_ifs("secret_key", ios::binary);
    secret_key.load(context, secret_key_ifs);
    secret_key_ifs.close();
    // Create a KeyGenerator using the loaded SecretKey
    KeyGenerator keygen(context, secret_key);
    // Generate a public key
    PublicKey public_key;
    keygen.create_public_key(public_key);
    // Generate relin keys
    RelinKeys relin_keys;
    keygen.create_relin_keys(relin_keys);
    // Generate galois keys
    GaloisKeys galois_keys;
    keygen.create_galois_keys(galois_keys);

    // Get clear data
    size_t slot_count =
        (*context.key_context_data()).parms().poly_modulus_degree();
    size_t row_size = slot_count / 2;
    vector<uint64_t> x(slot_count, 0ULL);
    x[0] = 0ULL;
    x[1] = 1ULL;
    x[2] = 2ULL;
    x[3] = 3ULL;
    x[row_size] = 4ULL;
    x[row_size + 1] = 5ULL;
    x[row_size + 2] = 6ULL;
    x[row_size + 3] = 7ULL;
    Scalar shift = 4ULL;
    // Encode and encrypt
    ClearInputs clear_inputs{{"x", x}, {"shift", shift}};
    EncodedInputs encoded_inputs;
    polynomial_fct0_inputs_setup(context, public_key, clear_inputs,
                                 encoded_inputs);
    // Evaluate
    EncodedOutputs encoded_outputs;
    polynomial_fct0(context, relin_keys, galois_keys, encoded_inputs,
                    encoded_outputs);
    // Decrypt and decode
    ClearOutputs clear_outputs;
    polynomial_fct0_ouputs_setup(context, secret_key, encoded_outputs,
                                 clear_outputs);
    // Show results
    // How much noise budget do we have left?
    Decryptor decryptor(context, secret_key);
    cout << "    + Noise budget in result: "
         << decryptor.invariant_noise_budget(
                get<Ciphertext>(encoded_outputs["y"]))
         << " bits" << endl;
    cout << "    + Results of homomorphic computation" << endl;
    print_matrix(get<vector<Scalar>>(clear_outputs["y"]), row_size);
    vector<uint64_t> y_correct = x;
    transform(y_correct.begin(), y_correct.end(), y_correct.begin(),
              &scalar_polynomial_fct0);
    cout << "    + Results of clear computation" << endl;
    ;
    print_matrix(y_correct, row_size);
    return 0;
}