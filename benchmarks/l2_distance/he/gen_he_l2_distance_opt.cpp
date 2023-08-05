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
Ciphertext c2058 = encrypted_inputs.at("c2");
Ciphertext c2057 = encrypted_inputs.at("c1");
evaluator.sub(c2057, c2058, c2057);
evaluator.square(c2057, c2057);
evaluator.relinearize(c2057, relin_keys, c2057);
evaluator.rotate_rows(c2057, 512, galois_keys, c2058);
evaluator.add(c2058, c2057, c2058);
evaluator.rotate_rows(c2058, 256, galois_keys, c2057);
evaluator.add(c2057, c2058, c2057);
evaluator.rotate_rows(c2057, 128, galois_keys, c2058);
evaluator.add(c2058, c2057, c2058);
evaluator.rotate_rows(c2058, 64, galois_keys, c2057);
evaluator.add(c2057, c2058, c2057);
evaluator.rotate_rows(c2057, 32, galois_keys, c2058);
evaluator.add(c2058, c2057, c2058);
evaluator.rotate_rows(c2058, 16, galois_keys, c2057);
evaluator.add(c2057, c2058, c2057);
evaluator.rotate_rows(c2057, 8, galois_keys, c2058);
evaluator.add(c2058, c2057, c2058);
evaluator.rotate_rows(c2058, 4, galois_keys, c2057);
evaluator.add(c2057, c2058, c2057);
evaluator.rotate_rows(c2057, 2, galois_keys, c2058);
evaluator.add(c2058, c2057, c2058);
evaluator.rotate_rows(c2058, 1, galois_keys, c2057);
evaluator.add(c2057, c2058, c2057);
encrypted_outputs.emplace("result", move(c2057));
}

vector<int> get_rotation_steps_l2_distance_opt(){
return vector<int>{512, 256, 128, 64, 32, 16, 8, 4, 2, 1};
}
