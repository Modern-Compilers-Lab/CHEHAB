#include "gen_he_hamming_distance_noopt.hpp"
#include <cstddef>
#include <cstdint>
#include <utility>

using namespace std;
using namespace seal;

void hamming_distance_noopt(
  const unordered_map<string, Ciphertext> &encrypted_inputs, const unordered_map<string, Plaintext> &encoded_inputs,
  unordered_map<string, Ciphertext> &encrypted_outputs, unordered_map<string, Plaintext> &encoded_outputs,
  const BatchEncoder &encoder, const Encryptor &encryptor, const Evaluator &evaluator, const RelinKeys &relin_keys,
  const GaloisKeys &galois_keys)
{
  Ciphertext c3 = encrypted_inputs.at("c1");
  Ciphertext c2 = encrypted_inputs.at("c0");
  size_t slot_count = encoder.slot_count();
  Plaintext p1;
  encoder.encode(vector<std::int64_t>(slot_count, 2), p1);
  Ciphertext c4;
  evaluator.add(c3, c2, c4);
  Ciphertext c5;
  evaluator.negate(c4, c5);
  evaluator.multiply(c5, c3, c5);
  evaluator.relinearize(c5, relin_keys, c5);
  evaluator.multiply(c4, c2, c2);
  evaluator.relinearize(c2, relin_keys, c2);
  evaluator.sub(c5, c2, c5);
  evaluator.multiply_plain(c4, p1, c4);
  evaluator.add(c5, c4, c5);
  evaluator.rotate_rows(c5, 2, galois_keys, c4);
  evaluator.add(c4, c5, c4);
  evaluator.rotate_rows(c4, 1, galois_keys, c5);
  evaluator.add(c5, c4, c5);
  encrypted_outputs.emplace("c_result", move(c5));
}

vector<int> get_rotation_steps_hamming_distance_noopt()
{
  return vector<int>{2, 1};
}
