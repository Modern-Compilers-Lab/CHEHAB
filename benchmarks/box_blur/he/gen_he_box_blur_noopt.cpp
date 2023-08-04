#include <cstddef>
#include <cstdint>
#include <utility>
#include "gen_he_box_blur_noopt.hpp"

using namespace std;
using namespace seal;

void box_blur_noopt(const unordered_map<string, Ciphertext> &encrypted_inputs,
const unordered_map<string, Plaintext> &encoded_inputs,
unordered_map<string, Ciphertext> &encrypted_outputs,
unordered_map<string, Plaintext> &encoded_outputs,
const BatchEncoder &encoder,
const Encryptor &encryptor,
const Evaluator &evaluator,
const RelinKeys &relin_keys,
const GaloisKeys &galois_keys)
{
Ciphertext c1 = encrypted_inputs.at("img");
Ciphertext c2;
evaluator.rotate_rows(c1, 991, galois_keys, c2);
Ciphertext c3;
evaluator.rotate_rows(c1, 992, galois_keys, c3);
evaluator.add(c2, c3, c2);
evaluator.rotate_rows(c1, 993, galois_keys, c3);
evaluator.add(c2, c3, c2);
evaluator.rotate_rows(c1, 1023, galois_keys, c3);
evaluator.add(c3, c1, c3);
Ciphertext c9;
evaluator.rotate_rows(c1, 1, galois_keys, c9);
evaluator.add(c3, c9, c3);
evaluator.add(c2, c3, c2);
evaluator.rotate_rows(c1, 31, galois_keys, c3);
evaluator.rotate_rows(c1, 32, galois_keys, c9);
evaluator.add(c3, c9, c3);
evaluator.rotate_rows(c1, 33, galois_keys, c1);
evaluator.add(c3, c1, c3);
evaluator.add(c2, c3, c2);
encrypted_outputs.emplace("result", move(c2));
}

vector<int> get_rotation_steps_box_blur_noopt(){
return vector<int>{991, 992, 993, 31, 1023, 1, 32, 33};
}
