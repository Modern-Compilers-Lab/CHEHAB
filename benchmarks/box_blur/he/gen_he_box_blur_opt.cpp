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
Ciphertext c18 = encrypted_inputs.at("img");
Ciphertext c26;
evaluator.rotate_rows(c18, 1, galois_keys, c26);
Ciphertext c35;
evaluator.add(c18, c26, c35);
Ciphertext c37;
evaluator.rotate_rows(c18, 2, galois_keys, c37);
evaluator.add(c35, c37, c35);
evaluator.rotate_rows(c35, 31, galois_keys, c37);
Ciphertext c24;
evaluator.rotate_rows(c18, 1023, galois_keys, c24);
evaluator.add(c18, c24, c18);
evaluator.add(c18, c26, c18);
evaluator.rotate_rows(c35, 991, galois_keys, c35);
evaluator.add(c18, c35, c18);
evaluator.add(c37, c18, c37);
encrypted_outputs.emplace("result", move(c37));
}

vector<int> get_rotation_steps_box_blur_opt(){
return vector<int>{1, 2, 31, 1023, 991};
}
