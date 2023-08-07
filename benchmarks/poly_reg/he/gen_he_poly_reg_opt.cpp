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
Ciphertext c19 = encrypted_inputs.at("c4");
Ciphertext c18 = encrypted_inputs.at("c3");
Ciphertext c17 = encrypted_inputs.at("c2");
Ciphertext c15 = encrypted_inputs.at("c0");
Ciphertext c16 = encrypted_inputs.at("c1");
evaluator.multiply(c15, c19, c19);
evaluator.add(c18, c19, c18);
evaluator.relinearize(c18, relin_keys, c18);
evaluator.multiply(c15, c18, c15);
evaluator.add(c17, c15, c17);
evaluator.sub(c16, c17, c16);
encrypted_outputs.emplace("c_result", move(c16));
}

vector<int> get_rotation_steps_poly_reg_opt(){
return vector<int>{};
}
