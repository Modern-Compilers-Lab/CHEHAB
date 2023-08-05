#include <cstddef>
#include <cstdint>
#include <utility>
#include "gen_he_gy_kernel_opt.hpp"

using namespace std;
using namespace seal;

void gy_kernel_opt(const unordered_map<string, Ciphertext> &encrypted_inputs,
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
Plaintext p68;
encoder.encode(vector<std::int64_t>(slot_count, 2), p68);
Plaintext p51;
encoder.encode(vector<std::int64_t>(slot_count, -2), p51);
Ciphertext c77;
evaluator.rotate_rows(c45, 33, galois_keys, c77);
Ciphertext c76;
evaluator.rotate_rows(c45, 31, galois_keys, c76);
Ciphertext c47;
evaluator.rotate_rows(c45, 32, galois_keys, c47);
evaluator.multiply_plain(c47, p68, c47);
Ciphertext c46;
evaluator.rotate_rows(c45, 992, galois_keys, c46);
evaluator.multiply_plain(c46, p51, c46);
Ciphertext c78;
evaluator.rotate_rows(c45, 991, galois_keys, c78);
evaluator.sub(c46, c78, c46);
evaluator.rotate_rows(c45, 993, galois_keys, c45);
evaluator.sub(c46, c45, c46);
evaluator.add(c47, c46, c47);
evaluator.add(c76, c47, c76);
evaluator.add(c77, c76, c77);
encrypted_outputs.emplace("result", move(c77));
}

vector<int> get_rotation_steps_gy_kernel_opt(){
return vector<int>{33, 31, 993, 32, 992, 991};
}
