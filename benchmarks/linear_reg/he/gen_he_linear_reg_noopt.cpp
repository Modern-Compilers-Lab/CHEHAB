#include <cstddef>
#include <cstdint>
#include <utility>
#include "gen_he_linear_reg_noopt.hpp"

using namespace std;
using namespace seal;

void linear_reg_noopt(const unordered_map<string, Ciphertext> &encrypted_inputs,
const unordered_map<string, Plaintext> &encoded_inputs,
unordered_map<string, Ciphertext> &encrypted_outputs,
unordered_map<string, Plaintext> &encoded_outputs,
const BatchEncoder &encoder,
const Encryptor &encryptor,
const Evaluator &evaluator,
const RelinKeys &relin_keys,
const GaloisKeys &galois_keys)
{
Ciphertext c4 = encrypted_inputs.at("c3");
Ciphertext c3 = encrypted_inputs.at("c2");
Ciphertext c1 = encrypted_inputs.at("c0");
Ciphertext c2 = encrypted_inputs.at("c1");
evaluator.multiply(c3, c1, c3);
evaluator.relinearize(c3, relin_keys, c3);
evaluator.sub(c2, c3, c2);
evaluator.sub(c2, c4, c2);
encrypted_outputs.emplace("c_result", move(c2));
}

vector<int> get_rotation_steps_linear_reg_noopt(){
return vector<int>{};
}
