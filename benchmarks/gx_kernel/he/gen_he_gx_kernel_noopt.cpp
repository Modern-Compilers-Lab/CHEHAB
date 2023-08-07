#include <cstddef>
#include <cstdint>
#include <utility>
#include "gen_he_gx_kernel_noopt.hpp"

using namespace std;
using namespace seal;

void gx_kernel_noopt(const unordered_map<string, Ciphertext> &encrypted_inputs,
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
size_t slot_count = encoder.slot_count();
Plaintext p30;
encoder.encode(vector<std::int64_t>(slot_count, -1), p30);
Plaintext p25;
encoder.encode(vector<std::int64_t>(slot_count, 1), p25);
Plaintext p20;
encoder.encode(vector<std::int64_t>(slot_count, -2), p20);
Plaintext p7;
encoder.encode(vector<std::int64_t>(slot_count, 0), p7);
Plaintext p17;
encoder.encode(vector<std::int64_t>(slot_count, 0), p17);
Plaintext p27;
encoder.encode(vector<std::int64_t>(slot_count, 0), p27);
Plaintext p5;
encoder.encode(vector<std::int64_t>(slot_count, 1), p5);
Plaintext p10;
encoder.encode(vector<std::int64_t>(slot_count, -1), p10);
Plaintext p15;
encoder.encode(vector<std::int64_t>(slot_count, 2), p15);
Ciphertext c2;
evaluator.rotate_rows(c1, 4032, galois_keys, c2);
Ciphertext c9;
evaluator.rotate_rows(c2, 4095, galois_keys, c9);
evaluator.multiply_plain(c9, p10, c9);
Ciphertext c37;
evaluator.multiply_plain(c2, p7, c37);
evaluator.add(c9, c37, c9);
evaluator.rotate_rows(c2, 1, galois_keys, c2);
evaluator.multiply_plain(c2, p5, c2);
evaluator.add(c9, c2, c9);
evaluator.rotate_rows(c1, 4095, galois_keys, c2);
evaluator.multiply_plain(c2, p20, c2);
evaluator.multiply_plain(c1, p17, c37);
evaluator.add(c2, c37, c2);
evaluator.rotate_rows(c1, 1, galois_keys, c37);
evaluator.multiply_plain(c37, p15, c37);
evaluator.add(c2, c37, c2);
evaluator.add(c9, c2, c9);
evaluator.rotate_rows(c1, 64, galois_keys, c1);
evaluator.rotate_rows(c1, 4095, galois_keys, c2);
evaluator.multiply_plain(c2, p30, c2);
evaluator.multiply_plain(c1, p27, c37);
evaluator.add(c2, c37, c2);
evaluator.rotate_rows(c1, 1, galois_keys, c1);
evaluator.multiply_plain(c1, p25, c1);
evaluator.add(c2, c1, c2);
evaluator.add(c9, c2, c9);
encrypted_outputs.emplace("result", move(c9));
}

vector<int> get_rotation_steps_gx_kernel_noopt(){
return vector<int>{4095, 4032, 1, 64};
}
