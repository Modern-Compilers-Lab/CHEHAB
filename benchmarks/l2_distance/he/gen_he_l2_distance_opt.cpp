#include <cstddef>
#include <cstdint>
#include <utility>
#include "gen_he_l2_distance_opt.hpp"

using namespace std;
using namespace seal;

void l2_distance_opt(const unordered_map<string, Ciphertext> &encrypted_inputs,
const unordered_map<string, Plaintext> &encoded_inputs,
unordered_map<string, Ciphertext> &encrypted_outputs,
unordered_map<string, Plaintext> &encoded_outputs,
const BatchEncoder &encoder,
const Encryptor &encryptor,
const Evaluator &evaluator,
const RelinKeys &relin_keys,
const GaloisKeys &galois_keys)
{
Ciphertext c8202 = encrypted_inputs.at("c2");
Ciphertext c8201 = encrypted_inputs.at("c1");
evaluator.sub(c8201, c8202, c8201);
evaluator.square(c8201, c8201);
evaluator.relinearize(c8201, relin_keys, c8201);
evaluator.rotate_rows(c8201, 2048, galois_keys, c8202);
evaluator.add(c8202, c8201, c8202);
evaluator.rotate_rows(c8202, 1024, galois_keys, c8201);
evaluator.add(c8201, c8202, c8201);
evaluator.rotate_rows(c8201, 512, galois_keys, c8202);
evaluator.add(c8202, c8201, c8202);
evaluator.rotate_rows(c8202, 256, galois_keys, c8201);
evaluator.add(c8201, c8202, c8201);
evaluator.rotate_rows(c8201, 128, galois_keys, c8202);
evaluator.add(c8202, c8201, c8202);
evaluator.rotate_rows(c8202, 64, galois_keys, c8201);
evaluator.add(c8201, c8202, c8201);
evaluator.rotate_rows(c8201, 32, galois_keys, c8202);
evaluator.add(c8202, c8201, c8202);
evaluator.rotate_rows(c8202, 16, galois_keys, c8201);
evaluator.add(c8201, c8202, c8201);
evaluator.rotate_rows(c8201, 8, galois_keys, c8202);
evaluator.add(c8202, c8201, c8202);
evaluator.rotate_rows(c8202, 4, galois_keys, c8201);
evaluator.add(c8201, c8202, c8201);
evaluator.rotate_rows(c8201, 2, galois_keys, c8202);
evaluator.add(c8202, c8201, c8202);
evaluator.rotate_rows(c8202, 1, galois_keys, c8201);
evaluator.add(c8201, c8202, c8201);
encrypted_outputs.emplace("result", move(c8201));
}

vector<int> get_rotation_steps_l2_distance_opt(){
return vector<int>{4, 8, 16, 32, 64, 1024, 2, 2048, 1, 128, 512, 256};
}
