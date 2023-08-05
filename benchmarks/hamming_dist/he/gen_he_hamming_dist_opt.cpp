#include <cstddef>
#include <cstdint>
#include <utility>
#include "gen_he_hamming_dist_opt.hpp"

using namespace std;
using namespace seal;

void hamming_dist_opt(const unordered_map<string, Ciphertext> &encrypted_inputs,
const unordered_map<string, Plaintext> &encoded_inputs,
unordered_map<string, Ciphertext> &encrypted_outputs,
unordered_map<string, Plaintext> &encoded_outputs,
const BatchEncoder &encoder,
const Encryptor &encryptor,
const Evaluator &evaluator,
const RelinKeys &relin_keys,
const GaloisKeys &galois_keys)
{
Ciphertext c2061 = encrypted_inputs.at("c2");
Ciphertext c2060 = encrypted_inputs.at("c1");
size_t slot_count = encoder.slot_count();
Plaintext p2063;
encoder.encode(vector<std::int64_t>(slot_count, 2), p2063);
Ciphertext c2062;
evaluator.add(c2060, c2061, c2062);
evaluator.multiply(c2060, c2061, c2060);
evaluator.multiply_plain(c2060, p2063, c2060);
evaluator.sub(c2062, c2060, c2062);
evaluator.relinearize(c2062, relin_keys, c2062);
evaluator.rotate_rows(c2062, 512, galois_keys, c2060);
evaluator.add(c2062, c2060, c2062);
evaluator.rotate_rows(c2062, 256, galois_keys, c2060);
evaluator.add(c2062, c2060, c2062);
evaluator.rotate_rows(c2062, 128, galois_keys, c2060);
evaluator.add(c2062, c2060, c2062);
evaluator.rotate_rows(c2062, 64, galois_keys, c2060);
evaluator.add(c2062, c2060, c2062);
evaluator.rotate_rows(c2062, 32, galois_keys, c2060);
evaluator.add(c2062, c2060, c2062);
evaluator.rotate_rows(c2062, 16, galois_keys, c2060);
evaluator.add(c2062, c2060, c2062);
evaluator.rotate_rows(c2062, 8, galois_keys, c2060);
evaluator.add(c2062, c2060, c2062);
evaluator.rotate_rows(c2062, 4, galois_keys, c2060);
evaluator.add(c2062, c2060, c2062);
evaluator.rotate_rows(c2062, 2, galois_keys, c2060);
evaluator.add(c2062, c2060, c2062);
evaluator.rotate_rows(c2062, 1, galois_keys, c2060);
evaluator.add(c2062, c2060, c2062);
encrypted_outputs.emplace("result", move(c2062));
}

vector<int> get_rotation_steps_hamming_dist_opt(){
return vector<int>{512, 256, 128, 64, 32, 16, 8, 4, 2, 1};
}
