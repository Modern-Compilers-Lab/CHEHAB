#include <cstddef>
#include <cstdint>
#include <utility>
#include "gen_he_box_blur_opt.hpp"

using namespace std;
using namespace seal;

void box_blur_opt(const unordered_map<string, Ciphertext> &encrypted_inputs,
const unordered_map<string, Plaintext> &encoded_inputs,
unordered_map<string, Ciphertext> &encrypted_outputs,
unordered_map<string, Plaintext> &encoded_outputs,
const BatchEncoder &encoder,
const Encryptor &encryptor,
const Evaluator &evaluator,
const RelinKeys &relin_keys,
const GaloisKeys &galois_keys)
{
Ciphertext c45 = encrypted_inputs.at("img");
Ciphertext c76;
evaluator.rotate_rows(c45, 2, galois_keys, c76);
Ciphertext c56;
evaluator.rotate_rows(c45, 1, galois_keys, c56);
Ciphertext c83;
evaluator.add(c45, c56, c83);
evaluator.add(c76, c83, c76);
evaluator.rotate_rows(c76, 63, galois_keys, c83);
Ciphertext c59;
evaluator.rotate_rows(c45, 4095, galois_keys, c59);
evaluator.add(c45, c59, c45);
evaluator.add(c56, c45, c56);
evaluator.rotate_rows(c76, 4031, galois_keys, c76);
evaluator.add(c56, c76, c56);
evaluator.add(c83, c56, c83);
encrypted_outputs.emplace("result", move(c83));
}

vector<int> get_rotation_steps_box_blur_opt(){
return vector<int>{1, 4031, 2, 63, 4095};
}
