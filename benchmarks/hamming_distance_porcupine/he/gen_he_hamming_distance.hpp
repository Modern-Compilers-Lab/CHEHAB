#include <cstdint>
#include <unordered_map>
#include <utility>
#include <vector>
#include "seal/seal.h"

seal::SEALContext create_context()
{
  seal::EncryptionParameters params(seal::scheme_type::bfv);
  params.set_poly_modulus_degree(4096);
  params.set_plain_modulus(seal::PlainModulus::Batching(4096, 20));
  params.set_coeff_modulus(seal::CoeffModulus::Create(4096, {36, 36, 37}));
  seal::SEALContext context(params, true, seal::sec_level_type::tc128);
  return context;
}

std::vector<int> get_rotations_steps()
{
  std::vector<int> steps = {1, 2};
  return steps;
}

void hamming_distance_baseline(
  const std::unordered_map<std::string, seal::Ciphertext> &encrypted_inputs,
  const std::unordered_map<std::string, seal::Plaintext> &encoded_inputs,
  std::unordered_map<std::string, seal::Ciphertext> &encrypted_outputs,
  std::unordered_map<std::string, seal::Plaintext> &encoded_outputs, const seal::BatchEncoder &encoder,
  const seal::Encryptor &encryptor, const seal::Evaluator &evaluator, const seal::RelinKeys &relin_keys,
  const seal::GaloisKeys &galois_keys)
{
  seal::Ciphertext c0 = encrypted_inputs.at("c0");
  seal::Ciphertext c1 = encrypted_inputs.at("c1");
  std::vector<std::int64_t> p0_clear(4096, 2);
  seal::Plaintext p0;
  encoder.encode(p0_clear, p0);
  seal::Ciphertext c2;
  evaluator.add(c1, c0, c2);
  seal::Ciphertext c3;
  evaluator.negate(c2, c3);
  seal::Ciphertext c4;
  evaluator.multiply(c3, c1, c4);
  evaluator.relinearize_inplace(c4, relin_keys);
  seal::Ciphertext c5;
  evaluator.multiply(c2, c0, c5);
  evaluator.relinearize_inplace(c5, relin_keys);
  seal::Ciphertext c6;
  evaluator.multiply_plain(c2, p0, c6);
  seal::Ciphertext c7;
  evaluator.sub(c4, c5, c7);
  seal::Ciphertext c8;
  evaluator.add(c7, c6, c8);
  seal::Ciphertext c9;
  evaluator.rotate_rows(c8, 2, galois_keys, c9);
  seal::Ciphertext c10;
  evaluator.add(c9, c8, c10);
  seal::Ciphertext c11;
  evaluator.rotate_rows(c10, 1, galois_keys, c11);
  seal::Ciphertext c12;
  evaluator.add(c11, c10, c12);
  encrypted_outputs.emplace("c_result", std::move(c12));
}

void hamming_distance_opt1(
  const std::unordered_map<std::string, seal::Ciphertext> &encrypted_inputs,
  const std::unordered_map<std::string, seal::Plaintext> &encoded_inputs,
  std::unordered_map<std::string, seal::Ciphertext> &encrypted_outputs,
  std::unordered_map<std::string, seal::Plaintext> &encoded_outputs, const seal::BatchEncoder &encoder,
  const seal::Encryptor &encryptor, const seal::Evaluator &evaluator, const seal::RelinKeys &relin_keys,
  const seal::GaloisKeys &galois_keys)
{
  seal::Ciphertext c0 = encrypted_inputs.at("c0");
  seal::Ciphertext c1 = encrypted_inputs.at("c1");
  std::vector<std::int64_t> p0_clear(4096, 2);
  seal::Plaintext p0;
  encoder.encode(p0_clear, p0);
  seal::Ciphertext c2;
  evaluator.add(c1, c0, c2);
  seal::Ciphertext c3;
  evaluator.negate(c2, c3);
  seal::Ciphertext c4;
  evaluator.add_plain(c3, p0, c4);
  seal::Ciphertext c5;
  evaluator.multiply(c4, c2, c5);
  evaluator.relinearize_inplace(c5, relin_keys);
  seal::Ciphertext c6;
  evaluator.rotate_rows(c5, 2, galois_keys, c6);
  seal::Ciphertext c7;
  evaluator.add(c6, c5, c7);
  seal::Ciphertext c8;
  evaluator.rotate_rows(c7, 1, galois_keys, c8);
  seal::Ciphertext c9;
  evaluator.add(c8, c7, c9);
  encrypted_outputs.emplace("c_result", std::move(c9));
}
