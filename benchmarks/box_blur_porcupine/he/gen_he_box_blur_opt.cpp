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
Ciphertext c8 = encrypted_inputs.at("c0");
Ciphertext c9;
evaluator.rotate_rows(c8, 1, galois_keys, c9);
evaluator.add(c8, c9, c8);
evaluator.rotate_rows(c8, 5, galois_keys, c9);
evaluator.add(c8, c9, c8);
encrypted_outputs.emplace("c6", move(c8));
}

vector<int> get_rotation_steps_box_blur_opt(){
return vector<int>{1, 5};
}
