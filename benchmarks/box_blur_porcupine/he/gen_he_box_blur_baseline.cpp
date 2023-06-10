#include <cstddef>
#include <cstdint>
#include <utility>
#include "gen_he_box_blur.hpp"

using namespace std;
using namespace seal;

void box_blur_baseline(const unordered_map<string, Ciphertext> &encrypted_inputs,
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
evaluator.add(c2, c1, c5);
Ciphertext c3;
evaluator.rotate_rows(c1, 5, galois_keys, c3);
Ciphertext c4;
evaluator.rotate_rows(c1, 6, galois_keys, c4);
Ciphertext c6;
evaluator.add(c3, c4, c6);
Ciphertext c7;
evaluator.add(c5, c6, c7);
encrypted_outputs.emplace("c6", move(c7));
}

vector<int> get_rotation_steps_box_blur_baseline(){
return vector<int>{6, 5, 1};
}
