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
Plaintext p2055 = encoded_inputs.at("v1");
Ciphertext c2054 = encrypted_inputs.at("c0");
evaluator.multiply_plain(c2054, p2055, c2054);
Ciphertext c3082;
evaluator.rotate_rows(c2054, 512, galois_keys, c3082);
evaluator.add(c2054, c3082, c2054);
evaluator.rotate_rows(c2054, 256, galois_keys, c3082);
evaluator.add(c2054, c3082, c2054);
evaluator.rotate_rows(c2054, 128, galois_keys, c3082);
evaluator.add(c2054, c3082, c2054);
evaluator.rotate_rows(c2054, 64, galois_keys, c3082);
evaluator.add(c2054, c3082, c2054);
evaluator.rotate_rows(c2054, 32, galois_keys, c3082);
evaluator.add(c2054, c3082, c2054);
evaluator.rotate_rows(c2054, 16, galois_keys, c3082);
evaluator.add(c2054, c3082, c2054);
evaluator.rotate_rows(c2054, 8, galois_keys, c3082);
evaluator.add(c2054, c3082, c2054);
evaluator.rotate_rows(c2054, 4, galois_keys, c3082);
evaluator.add(c2054, c3082, c2054);
evaluator.rotate_rows(c2054, 2, galois_keys, c3082);
evaluator.add(c2054, c3082, c2054);
evaluator.rotate_rows(c2054, 1, galois_keys, c3082);
evaluator.add(c2054, c3082, c2054);
encrypted_outputs.emplace("result", move(c2054));
}

vector<int> get_rotation_steps_dot_product_opt(){
return vector<int>{512, 256, 128, 64, 32, 16, 8, 4, 2, 1};
}
