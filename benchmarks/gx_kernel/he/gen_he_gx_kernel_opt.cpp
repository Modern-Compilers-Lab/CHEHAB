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
encoder.encode(vector<std::int64_t>(slot_count, 2), p63);
Plaintext p58;
encoder.encode(vector<std::int64_t>(slot_count, -2), p58);
Ciphertext c80;
evaluator.rotate_rows(c45, 2, galois_keys, c80);
evaluator.sub(c80, c45, c80);
Ciphertext c82;
evaluator.rotate_rows(c80, 31, galois_keys, c82);
Ciphertext c59;
evaluator.rotate_rows(c45, 1023, galois_keys, c59);
evaluator.multiply_plain(c59, p58, c59);
evaluator.rotate_rows(c45, 1, galois_keys, c45);
evaluator.multiply_plain(c45, p63, c45);
evaluator.add(c59, c45, c59);
evaluator.rotate_rows(c80, 991, galois_keys, c80);
evaluator.add(c59, c80, c59);
evaluator.add(c82, c59, c82);
encrypted_outputs.emplace("result", move(c82));
}

vector<int> get_rotation_steps_gx_kernel_opt(){
return vector<int>{2, 31, 1023, 1, 991};
}
