#include <cstddef>
#include <cstdint>
#include <utility>
#include "gen_he_box_blur.hpp"

using namespace std;
using namespace seal;

void box_blur(const unordered_map<string, Ciphertext> &encrypted_inputs,
const unordered_map<string, Plaintext> &encoded_inputs,
unordered_map<string, Ciphertext> &encrypted_outputs,
unordered_map<string, Plaintext> &encoded_outputs,
const BatchEncoder &encoder,
const Encryptor &encryptor,
const Evaluator &evaluator,
const RelinKeys &relin_keys,
const GaloisKeys &galois_keys)
{
Ciphertext c1 = encrypted_inputs.at("c0");
Ciphertext c2;
evaluator.rotate_rows(c1, 1, galois_keys, c2);
Ciphertext c5;
evaluator.add(c1, c2, c5);
Ciphertext c8;
evaluator.rotate_rows(c5, 5, galois_keys, c8);
Ciphertext c7;
evaluator.add(c5, c8, c7);
encrypted_outputs.emplace("c6", move(c7));
}

vector<int> get_rotation_steps_box_blur(){
return vector<int>{5, 1};
}
