#include <cstddef>
#include <cstdint>
#include <utility>
#include "gen_he_roberts_cross_opt.hpp"

using namespace std;
using namespace seal;

void roberts_cross_opt(const unordered_map<string, Ciphertext> &encrypted_inputs,
const unordered_map<string, Plaintext> &encoded_inputs,
unordered_map<string, Ciphertext> &encrypted_outputs,
unordered_map<string, Plaintext> &encoded_outputs,
const BatchEncoder &encoder,
const Encryptor &encryptor,
const Evaluator &evaluator,
const RelinKeys &relin_keys,
const GaloisKeys &galois_keys)
{
Ciphertext c42 = encrypted_inputs.at("img");
Ciphertext c44;
evaluator.rotate_rows(c42, 1, galois_keys, c44);
Ciphertext c43;
evaluator.rotate_rows(c42, 64, galois_keys, c43);
evaluator.sub(c44, c43, c44);
evaluator.square(c44, c44);
evaluator.rotate_rows(c42, 65, galois_keys, c43);
evaluator.sub(c42, c43, c42);
evaluator.square(c42, c42);
evaluator.add(c44, c42, c44);
encrypted_outputs.emplace("result", move(c44));
}

vector<int> get_rotation_steps_roberts_cross_opt(){
return vector<int>{64, 1, 65};
}
