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
Ciphertext c46;
evaluator.rotate_rows(c45, 4032, galois_keys, c46);
evaluator.multiply_plain(c46, p51, c46);
Ciphertext c80;
evaluator.rotate_rows(c45, 4031, galois_keys, c80);
evaluator.sub(c46, c80, c46);
evaluator.rotate_rows(c45, 4033, galois_keys, c80);
evaluator.sub(c46, c80, c46);
evaluator.rotate_rows(c45, 64, galois_keys, c80);
evaluator.multiply_plain(c80, p68, c80);
Ciphertext c77;
evaluator.rotate_rows(c45, 63, galois_keys, c77);
evaluator.add(c80, c77, c80);
evaluator.rotate_rows(c45, 65, galois_keys, c45);
evaluator.add(c80, c45, c80);
evaluator.add(c46, c80, c46);
encrypted_outputs.emplace("result", move(c46));
}

vector<int> get_rotation_steps_gy_kernel_opt(){
return vector<int>{64, 4033, 63, 4032, 4031, 65};
}
