#include "gen_he_poly_reg_noopt.hpp"
#include <cstddef>
#include <cstdint>
#include <utility>

using namespace std;
using namespace seal;

void poly_reg_noopt(
  const unordered_map<string, Ciphertext> &encrypted_inputs, const unordered_map<string, Plaintext> &encoded_inputs,
  unordered_map<string, Ciphertext> &encrypted_outputs, unordered_map<string, Plaintext> &encoded_outputs,
  const BatchEncoder &encoder, const Encryptor &encryptor, const Evaluator &evaluator, const RelinKeys &relin_keys,
  const GaloisKeys &galois_keys)
{
  Ciphertext c5 = encrypted_inputs.at("c4");
  Ciphertext c4 = encrypted_inputs.at("c3");
  Ciphertext c3 = encrypted_inputs.at("c2");
  Ciphertext c2 = encrypted_inputs.at("c1");
  Ciphertext c1 = encrypted_inputs.at("c0");
  size_t slot_count = encoder.slot_count();
  Ciphertext c12;
  evaluator.square(c1, c12);
  evaluator.relinearize(c12, relin_keys, c12);
  evaluator.multiply(c12, c5, c12);
  evaluator.relinearize(c12, relin_keys, c12);
  evaluator.multiply(c1, c4, c1);
  evaluator.relinearize(c1, relin_keys, c1);
  evaluator.add(c12, c1, c12);
  evaluator.add(c12, c3, c12);
  evaluator.sub(c2, c12, c2);
  encrypted_outputs.emplace("c_result", move(c2));
}

vector<int> get_rotation_steps_poly_reg_noopt()
{
  return vector<int>{};
}
