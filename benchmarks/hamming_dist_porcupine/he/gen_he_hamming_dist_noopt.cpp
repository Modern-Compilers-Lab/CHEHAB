#include <cstddef>
#include <cstdint>
#include <utility>
#include "gen_he_hamming_dist_noopt.hpp"

using namespace std;
using namespace seal;

void hamming_dist_noopt(const unordered_map<string, Ciphertext> &encrypted_inputs,
const unordered_map<string, Plaintext> &encoded_inputs,
unordered_map<string, Ciphertext> &encrypted_outputs,
unordered_map<string, Plaintext> &encoded_outputs,
const BatchEncoder &encoder,
const Encryptor &encryptor,
const Evaluator &evaluator,
const RelinKeys &relin_keys,
const GaloisKeys &galois_keys)
{
Ciphertext c3 = encrypted_inputs.at("c1");
Ciphertext c2 = encrypted_inputs.at("c0");
size_t slot_count = encoder.slot_count();
Plaintext p1;
encoder.encode(vector<std::int64_t>(slot_count, 2), p1);
Ciphertext c4;
evaluator.add(c3, c2, c4);
Ciphertext c7;
evaluator.negate(c4, c7);
evaluator.multiply(c7, c3, c7);
evaluator.relinearize(c7, relin_keys, c7);
evaluator.multiply(c4, c2, c2);
evaluator.relinearize(c2, relin_keys, c2);
evaluator.sub(c7, c2, c7);
evaluator.multiply_plain(c4, p1, c4);
evaluator.add(c7, c4, c7);
evaluator.rotate_rows(c7, 2, galois_keys, c4);
evaluator.add(c4, c7, c4);
evaluator.rotate_rows(c4, 1, galois_keys, c7);
evaluator.add(c7, c4, c7);
encrypted_outputs.emplace("c_result", move(c7));
}

vector<int> get_rotation_steps_hamming_dist_noopt(){
return vector<int>{2, 1};
}
