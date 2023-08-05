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
Plaintext p24;
encoder.encode(vector<std::int64_t>(slot_count, 0), p24);
Plaintext p11;
encoder.encode(vector<std::int64_t>(slot_count, -1), p11);
Plaintext p22;
encoder.encode(vector<std::int64_t>(slot_count, -1), p22);
Plaintext p9;
encoder.encode(vector<std::int64_t>(slot_count, 0), p9);
Plaintext p18;
encoder.encode(vector<std::int64_t>(slot_count, 1), p18);
Plaintext p5;
encoder.encode(vector<std::int64_t>(slot_count, 0), p5);
Plaintext p16;
encoder.encode(vector<std::int64_t>(slot_count, 0), p16);
Plaintext p3;
encoder.encode(vector<std::int64_t>(slot_count, 1), p3);
Ciphertext c34;
evaluator.multiply_plain(c1, p3, c34);
Ciphertext c6;
evaluator.rotate_rows(c1, 1, galois_keys, c6);
evaluator.multiply_plain(c6, p5, c6);
evaluator.add(c34, c6, c34);
evaluator.rotate_rows(c1, 64, galois_keys, c6);
Ciphertext c36;
evaluator.multiply_plain(c6, p9, c36);
Ciphertext c12;
evaluator.rotate_rows(c6, 1, galois_keys, c12);
evaluator.multiply_plain(c12, p11, c12);
evaluator.add(c36, c12, c36);
evaluator.add(c34, c36, c34);
evaluator.multiply(c34, c34, c34);
evaluator.relinearize(c34, relin_keys, c34);
evaluator.multiply_plain(c1, p16, c12);
evaluator.rotate_rows(c1, 1, galois_keys, c1);
evaluator.multiply_plain(c1, p18, c1);
evaluator.add(c12, c1, c12);
evaluator.multiply_plain(c6, p22, c1);
evaluator.rotate_rows(c6, 1, galois_keys, c6);
evaluator.multiply_plain(c6, p24, c6);
evaluator.add(c1, c6, c1);
evaluator.add(c12, c1, c12);
evaluator.multiply(c12, c12, c12);
evaluator.relinearize(c12, relin_keys, c12);
evaluator.add(c34, c12, c34);
encrypted_outputs.emplace("result", move(c34));
}

vector<int> get_rotation_steps_roberts_cross_noopt(){
return vector<int>{1, 64};
}
