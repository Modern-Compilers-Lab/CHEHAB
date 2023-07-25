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
Ciphertext c19 = encrypted_inputs.at("c1");
Ciphertext c18 = encrypted_inputs.at("c0");
size_t slot_count = encoder.slot_count();
Plaintext p17;
encoder.encode(vector<std::int64_t>(slot_count, 2), p17);
evaluator.add(c18, c19, c18);
evaluator.negate(c18, c19);
evaluator.add_plain(c19, p17, c19);
evaluator.multiply(c18, c19, c18);
evaluator.relinearize(c18, relin_keys, c18);
evaluator.rotate_rows(c18, 2, galois_keys, c19);
evaluator.add(c19, c18, c19);
evaluator.rotate_rows(c19, 1, galois_keys, c18);
evaluator.add(c18, c19, c18);
encrypted_outputs.emplace("c_result", move(c18));
}

vector<int> get_rotation_steps_hamming_dist_opt(){
return vector<int>{2, 1};
}
