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
Ciphertext c66;
evaluator.rotate_rows(c42, 65, galois_keys, c66);
evaluator.sub(c42, c66, c66);
evaluator.square(c66, c66);
Ciphertext c72;
evaluator.rotate_rows(c42, 63, galois_keys, c72);
evaluator.sub(c42, c72, c42);
evaluator.rotate_rows(c42, 1, galois_keys, c42);
evaluator.square(c42, c42);
evaluator.add(c66, c42, c66);
encrypted_outputs.emplace("result", move(c66));
}

vector<int> get_rotation_steps_roberts_cross_opt(){
return vector<int>{65, 63, 1};
}
