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
Ciphertext c8205 = encrypted_inputs.at("c2");
Ciphertext c8204 = encrypted_inputs.at("c1");
size_t slot_count = encoder.slot_count();
Plaintext p8207;
encoder.encode(vector<std::int64_t>(slot_count, 2), p8207);
Ciphertext c8209;
evaluator.add(c8204, c8205, c8209);
evaluator.multiply(c8204, c8205, c8204);
evaluator.multiply_plain(c8204, p8207, c8204);
evaluator.sub(c8209, c8204, c8209);
evaluator.relinearize(c8209, relin_keys, c8209);
evaluator.rotate_rows(c8209, 2048, galois_keys, c8204);
evaluator.add(c8209, c8204, c8209);
evaluator.rotate_rows(c8209, 1024, galois_keys, c8204);
evaluator.add(c8209, c8204, c8209);
evaluator.rotate_rows(c8209, 512, galois_keys, c8204);
evaluator.add(c8209, c8204, c8209);
evaluator.rotate_rows(c8209, 256, galois_keys, c8204);
evaluator.add(c8209, c8204, c8209);
evaluator.rotate_rows(c8209, 128, galois_keys, c8204);
evaluator.add(c8209, c8204, c8209);
evaluator.rotate_rows(c8209, 64, galois_keys, c8204);
evaluator.add(c8209, c8204, c8209);
evaluator.rotate_rows(c8209, 32, galois_keys, c8204);
evaluator.add(c8209, c8204, c8209);
evaluator.rotate_rows(c8209, 16, galois_keys, c8204);
evaluator.add(c8209, c8204, c8209);
evaluator.rotate_rows(c8209, 8, galois_keys, c8204);
evaluator.add(c8209, c8204, c8209);
evaluator.rotate_rows(c8209, 4, galois_keys, c8204);
evaluator.add(c8209, c8204, c8209);
evaluator.rotate_rows(c8209, 2, galois_keys, c8204);
evaluator.add(c8209, c8204, c8209);
evaluator.rotate_rows(c8209, 1, galois_keys, c8204);
evaluator.add(c8209, c8204, c8209);
encrypted_outputs.emplace("result", move(c8209));
}

vector<int> get_rotation_steps_hamming_dist_opt(){
return vector<int>{4, 8, 16, 32, 64, 1024, 2, 2048, 1, 128, 512, 256};
}
