#include <cstddef>
#include <cstdint>
#include <utility>
#include "gen_he_roberts_cross_noopt.hpp"

using namespace std;
using namespace seal;

void roberts_cross_noopt(const unordered_map<string, Ciphertext> &encrypted_inputs,
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
Plaintext p25;
encoder.encode(vector<std::int64_t>(slot_count, -1), p25);
Plaintext p19;
encoder.encode(vector<std::int64_t>(slot_count, 0), p19);
Plaintext p4;
encoder.encode(vector<std::int64_t>(slot_count, 0), p4);
Plaintext p10;
encoder.encode(vector<std::int64_t>(slot_count, -1), p10);
Plaintext p23;
encoder.encode(vector<std::int64_t>(slot_count, 0), p23);
Plaintext p12;
encoder.encode(vector<std::int64_t>(slot_count, 0), p12);
Plaintext p6;
encoder.encode(vector<std::int64_t>(slot_count, 1), p6);
Plaintext p17;
encoder.encode(vector<std::int64_t>(slot_count, 1), p17);
Ciphertext c34;
evaluator.multiply_plain(c1, p6, c34);
Ciphertext c3;
evaluator.rotate_rows(c1, 1, galois_keys, c3);
evaluator.multiply_plain(c3, p4, c3);
evaluator.add(c34, c3, c34);
evaluator.rotate_rows(c1, 64, galois_keys, c3);
Ciphertext c36;
evaluator.multiply_plain(c3, p12, c36);
Ciphertext c9;
evaluator.rotate_rows(c3, 1, galois_keys, c9);
evaluator.multiply_plain(c9, p10, c9);
evaluator.add(c36, c9, c36);
evaluator.add(c34, c36, c34);
evaluator.multiply(c34, c34, c34);
evaluator.relinearize(c34, relin_keys, c34);
evaluator.multiply_plain(c1, p19, c36);
evaluator.rotate_rows(c1, 1, galois_keys, c1);
evaluator.multiply_plain(c1, p17, c1);
evaluator.add(c36, c1, c36);
evaluator.multiply_plain(c3, p25, c1);
evaluator.rotate_rows(c3, 1, galois_keys, c3);
evaluator.multiply_plain(c3, p23, c3);
evaluator.add(c1, c3, c1);
evaluator.add(c36, c1, c36);
evaluator.multiply(c36, c36, c36);
evaluator.relinearize(c36, relin_keys, c36);
evaluator.add(c34, c36, c34);
encrypted_outputs.emplace("result", move(c34));
}

vector<int> get_rotation_steps_roberts_cross_noopt(){
return vector<int>{1, 64};
}
