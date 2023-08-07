#include <cstddef>
#include <cstdint>
#include <utility>
#include "gen_he_gx_kernel_opt.hpp"

using namespace std;
using namespace seal;

void gx_kernel_opt(const unordered_map<string, Ciphertext> &encrypted_inputs,
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
size_t slot_count = encoder.slot_count();
Plaintext p63;
encoder.encode(vector<std::int64_t>(slot_count, -2), p63);
Plaintext p59;
encoder.encode(vector<std::int64_t>(slot_count, 2), p59);
Ciphertext c80;
evaluator.rotate_rows(c45, 2, galois_keys, c80);
evaluator.sub(c80, c45, c80);
Ciphertext c82;
evaluator.rotate_rows(c80, 63, galois_keys, c82);
Ciphertext c58;
evaluator.rotate_rows(c45, 1, galois_keys, c58);
evaluator.multiply_plain(c58, p59, c58);
evaluator.rotate_rows(c45, 4095, galois_keys, c45);
evaluator.multiply_plain(c45, p63, c45);
evaluator.add(c58, c45, c58);
evaluator.rotate_rows(c80, 4031, galois_keys, c80);
evaluator.add(c58, c80, c58);
evaluator.add(c82, c58, c82);
encrypted_outputs.emplace("result", move(c82));
}

vector<int> get_rotation_steps_gx_kernel_opt(){
return vector<int>{63, 2, 1, 4031, 4095};
}
