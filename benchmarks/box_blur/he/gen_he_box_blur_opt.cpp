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
Ciphertext c60;
evaluator.rotate_rows(c45, 1, galois_keys, c60);
Ciphertext c73;
evaluator.add(c45, c60, c73);
Ciphertext c76;
evaluator.rotate_rows(c45, 2, galois_keys, c76);
evaluator.add(c73, c76, c73);
evaluator.rotate_rows(c73, 991, galois_keys, c76);
Ciphertext c56;
evaluator.rotate_rows(c45, 1023, galois_keys, c56);
evaluator.add(c45, c56, c45);
evaluator.add(c60, c45, c60);
evaluator.add(c76, c60, c76);
evaluator.rotate_rows(c73, 31, galois_keys, c73);
evaluator.add(c76, c73, c76);
encrypted_outputs.emplace("result", move(c76));
}

vector<int> get_rotation_steps_box_blur_opt(){
return vector<int>{1, 2, 991, 1023, 31};
}
