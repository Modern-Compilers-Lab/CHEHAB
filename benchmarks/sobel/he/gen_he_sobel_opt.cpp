#include <cstddef>
#include <cstdint>
#include <utility>
#include "gen_he_sobel_opt.hpp"

using namespace std;
using namespace seal;

void sobel_opt(const unordered_map<string, Ciphertext> &encrypted_inputs,
const unordered_map<string, Plaintext> &encoded_inputs,
unordered_map<string, Ciphertext> &encrypted_outputs,
unordered_map<string, Plaintext> &encoded_outputs,
const BatchEncoder &encoder,
const Encryptor &encryptor,
const Evaluator &evaluator,
const RelinKeys &relin_keys,
const GaloisKeys &galois_keys)
{
Ciphertext c91 = encrypted_inputs.at("img");
size_t slot_count = encoder.slot_count();
Plaintext p109;
encoder.encode(vector<std::int64_t>(slot_count, -2), p109);
Plaintext p105;
encoder.encode(vector<std::int64_t>(slot_count, 2), p105);
Ciphertext c92;
evaluator.rotate_rows(c91, 4032, galois_keys, c92);
evaluator.multiply_plain(c92, p109, c92);
Ciphertext c143;
evaluator.rotate_rows(c91, 4031, galois_keys, c143);
evaluator.sub(c92, c143, c92);
Ciphertext c141;
evaluator.rotate_rows(c91, 4033, galois_keys, c141);
evaluator.sub(c92, c141, c92);
Ciphertext c93;
evaluator.rotate_rows(c91, 64, galois_keys, c93);
evaluator.multiply_plain(c93, p105, c93);
Ciphertext c140;
evaluator.rotate_rows(c91, 63, galois_keys, c140);
evaluator.add(c93, c140, c93);
Ciphertext c139;
evaluator.rotate_rows(c91, 65, galois_keys, c139);
evaluator.add(c93, c139, c93);
evaluator.add(c92, c93, c92);
evaluator.square(c92, c92);
evaluator.sub(c139, c140, c139);
evaluator.rotate_rows(c91, 1, galois_keys, c140);
evaluator.multiply_plain(c140, p105, c140);
evaluator.rotate_rows(c91, 4095, galois_keys, c91);
evaluator.multiply_plain(c91, p109, c91);
evaluator.add(c140, c91, c140);
evaluator.sub(c141, c143, c141);
evaluator.add(c140, c141, c140);
evaluator.add(c139, c140, c139);
evaluator.square(c139, c139);
evaluator.add(c92, c139, c92);
encrypted_outputs.emplace("result", move(c92));
}

vector<int> get_rotation_steps_sobel_opt(){
return vector<int>{64, 63, 4033, 4032, 4031, 65, 1, 4095};
}
