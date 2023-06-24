#include "gen_he_hamming_distance.hpp"
#include <cstddef>
#include <cstdint>
#include <utility>

using namespace std;
using namespace seal;

void hamming_distance(
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
  evaluator.add(c2, c3, c2);
  evaluator.negate(c2, c3);
  evaluator.add_plain(c3, p1, c3);
  evaluator.multiply(c2, c3, c2);
  evaluator.relinearize(c2, relin_keys, c2);
  evaluator.rotate_rows(c2, 2, galois_keys, c3);
  evaluator.add(c2, c3, c2);
  evaluator.rotate_rows(c2, 1, galois_keys, c3);
  evaluator.add(c2, c3, c2);
  encrypted_outputs.emplace("c_result", move(c2));
}

vector<int> get_rotation_steps_hamming_distance()
{
  return vector<int>{2, 1};
}
