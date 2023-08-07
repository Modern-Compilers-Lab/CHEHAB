#include <cstddef>
#include <cstdint>
#include <utility>
#include "gen_he_linear_reg_opt.hpp"

using namespace std;
using namespace seal;

void linear_reg_opt(const unordered_map<string, Ciphertext> &encrypted_inputs,
const unordered_map<string, Plaintext> &encoded_inputs,
unordered_map<string, Ciphertext> &encrypted_outputs,
unordered_map<string, Plaintext> &encoded_outputs,
const BatchEncoder &encoder,
const Encryptor &encryptor,
const Evaluator &evaluator,
const RelinKeys &relin_keys,
const GaloisKeys &galois_keys)
{
Ciphertext c12 = encrypted_inputs.at("c3");
Ciphertext c11 = encrypted_inputs.at("c2");
Ciphertext c9 = encrypted_inputs.at("c0");
Ciphertext c10 = encrypted_inputs.at("c1");
evaluator.multiply(c9, c11, c9);
evaluator.sub(c10, c9, c10);
evaluator.sub(c10, c12, c10);
encrypted_outputs.emplace("c_result", move(c10));
}

vector<int> get_rotation_steps_linear_reg_opt(){
return vector<int>{};
}
