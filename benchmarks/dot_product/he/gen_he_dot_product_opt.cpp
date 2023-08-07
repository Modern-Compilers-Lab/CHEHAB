#include <cstddef>
#include <cstdint>
#include <utility>
#include "gen_he_dot_product_opt.hpp"

using namespace std;
using namespace seal;

void dot_product_opt(const unordered_map<string, Ciphertext> &encrypted_inputs,
const unordered_map<string, Plaintext> &encoded_inputs,
unordered_map<string, Ciphertext> &encrypted_outputs,
unordered_map<string, Plaintext> &encoded_outputs,
const BatchEncoder &encoder,
const Encryptor &encryptor,
const Evaluator &evaluator,
const RelinKeys &relin_keys,
const GaloisKeys &galois_keys)
{
Plaintext p8199 = encoded_inputs.at("v1");
Ciphertext c8198 = encrypted_inputs.at("c0");
evaluator.multiply_plain(c8198, p8199, c8198);
Ciphertext c12298;
evaluator.rotate_rows(c8198, 2048, galois_keys, c12298);
evaluator.add(c8198, c12298, c8198);
evaluator.rotate_rows(c8198, 1024, galois_keys, c12298);
evaluator.add(c8198, c12298, c8198);
evaluator.rotate_rows(c8198, 512, galois_keys, c12298);
evaluator.add(c8198, c12298, c8198);
evaluator.rotate_rows(c8198, 256, galois_keys, c12298);
evaluator.add(c8198, c12298, c8198);
evaluator.rotate_rows(c8198, 128, galois_keys, c12298);
evaluator.add(c8198, c12298, c8198);
evaluator.rotate_rows(c8198, 64, galois_keys, c12298);
evaluator.add(c8198, c12298, c8198);
evaluator.rotate_rows(c8198, 32, galois_keys, c12298);
evaluator.add(c8198, c12298, c8198);
evaluator.rotate_rows(c8198, 16, galois_keys, c12298);
evaluator.add(c8198, c12298, c8198);
evaluator.rotate_rows(c8198, 8, galois_keys, c12298);
evaluator.add(c8198, c12298, c8198);
evaluator.rotate_rows(c8198, 4, galois_keys, c12298);
evaluator.add(c8198, c12298, c8198);
evaluator.rotate_rows(c8198, 2, galois_keys, c12298);
evaluator.add(c8198, c12298, c8198);
evaluator.rotate_rows(c8198, 1, galois_keys, c12298);
evaluator.add(c8198, c12298, c8198);
encrypted_outputs.emplace("result", move(c8198));
}

vector<int> get_rotation_steps_dot_product_opt(){
return vector<int>{4, 8, 16, 32, 64, 1024, 2, 2048, 1, 128, 512, 256};
}
