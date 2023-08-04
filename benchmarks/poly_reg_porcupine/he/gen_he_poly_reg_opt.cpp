#include <cstddef>
#include <cstdint>
#include <utility>
#include "gen_he_poly_reg_opt.hpp"

using namespace std;
using namespace seal;

void poly_reg_opt(const unordered_map<string, Ciphertext> &encrypted_inputs,
const unordered_map<string, Plaintext> &encoded_inputs,
unordered_map<string, Ciphertext> &encrypted_outputs,
unordered_map<string, Plaintext> &encoded_outputs,
const BatchEncoder &encoder,
const Encryptor &encryptor,
const Evaluator &evaluator,
const RelinKeys &relin_keys,
const GaloisKeys &galois_keys)
{
Ciphertext c17 = encrypted_inputs.at("c4");
Ciphertext c16 = encrypted_inputs.at("c3");
Ciphertext c15 = encrypted_inputs.at("c2");
Ciphertext c13 = encrypted_inputs.at("c0");
Ciphertext c14 = encrypted_inputs.at("c1");
evaluator.multiply(c13, c17, c17);
evaluator.add(c16, c17, c16);
evaluator.relinearize(c16, relin_keys, c16);
evaluator.multiply(c13, c16, c13);
evaluator.add(c15, c13, c15);
evaluator.sub(c14, c15, c14);
encrypted_outputs.emplace("c_result", move(c14));
}

vector<int> get_rotation_steps_poly_reg_opt(){
return vector<int>{};
}
