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
encoder.encode(vector<std::int64_t>(slot_count, 2), p109);
Plaintext p104;
encoder.encode(vector<std::int64_t>(slot_count, -2), p104);
Ciphertext c141;
evaluator.rotate_rows(c91, 65, galois_keys, c141);
Ciphertext c139;
evaluator.rotate_rows(c91, 63, galois_keys, c139);
Ciphertext c142;
evaluator.sub(c141, c139, c142);
Ciphertext c105;
evaluator.rotate_rows(c91, 4095, galois_keys, c105);
evaluator.multiply_plain(c105, p104, c105);
Ciphertext c110;
evaluator.rotate_rows(c91, 1, galois_keys, c110);
evaluator.multiply_plain(c110, p109, c110);
evaluator.add(c105, c110, c105);
evaluator.rotate_rows(c91, 4033, galois_keys, c110);
Ciphertext c143;
evaluator.rotate_rows(c91, 4031, galois_keys, c143);
Ciphertext c146;
evaluator.sub(c110, c143, c146);
evaluator.add(c105, c146, c105);
evaluator.add(c142, c105, c142);
evaluator.square(c142, c142);
evaluator.rotate_rows(c91, 64, galois_keys, c146);
evaluator.multiply_plain(c146, p109, c146);
evaluator.add(c146, c139, c146);
evaluator.add(c141, c146, c141);
evaluator.rotate_rows(c91, 4032, galois_keys, c91);
evaluator.multiply_plain(c91, p104, c91);
evaluator.sub(c91, c143, c91);
evaluator.sub(c91, c110, c91);
evaluator.add(c141, c91, c141);
evaluator.square(c141, c141);
evaluator.add(c142, c141, c142);
encrypted_outputs.emplace("result", move(c142));
}

vector<int> get_rotation_steps_sobel_opt(){
return vector<int>{65, 4095, 63, 1, 4031, 4033, 64, 4032};
}
