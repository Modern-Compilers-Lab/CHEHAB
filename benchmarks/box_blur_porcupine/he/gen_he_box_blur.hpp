#include <cstdint>
#include <unordered_map>
#include <utility>
#include <vector>
#include "seal/seal.h"

seal::SEALContext create_context()
{
  seal::EncryptionParameters params(seal::scheme_type::bfv);
  params.set_poly_modulus_degree(4096);
  params.set_plain_modulus(seal::PlainModulus::Batching(4096, 16));
  params.set_coeff_modulus(seal::CoeffModulus::Create(4096, {54, 55}));
  seal::SEALContext context(params, false, seal::sec_level_type::tc128);
  return context;
}

std::vector<int> get_rotations_steps()
{
  std::vector<int> steps = {1, 5, 6};
  return steps;
}

void box_blur_baseline(
  const std::unordered_map<std::string, seal::Ciphertext> &encrypted_inputs,
  const std::unordered_map<std::string, seal::Plaintext> &encoded_inputs,
  std::unordered_map<std::string, seal::Ciphertext> &encrypted_outputs,
  std::unordered_map<std::string, seal::Plaintext> &encoded_outputs, const seal::BatchEncoder &encoder,
  const seal::Encryptor &encryptor, const seal::Evaluator &evaluator, const seal::RelinKeys &relin_keys,
  const seal::GaloisKeys &galois_keys)
{
  seal::Ciphertext c0 = encrypted_inputs.at("c0");
  seal::Ciphertext c1;
  evaluator.rotate_rows(c0, 1, galois_keys, c1);
  seal::Ciphertext c2;
  evaluator.add(c0, c1, c2);
  seal::Ciphertext c3;
  evaluator.rotate_rows(c0, 5, galois_keys, c3);
  seal::Ciphertext c4;
  evaluator.rotate_rows(c0, 6, galois_keys, c4);
  seal::Ciphertext c5;
  evaluator.add(c3, c4, c5);
  seal::Ciphertext c6;
  evaluator.add(c2, c5, c6);
  encrypted_outputs.emplace("c6", std::move(c6));
}

void box_blur_opt1(
  const std::unordered_map<std::string, seal::Ciphertext> &encrypted_inputs,
  const std::unordered_map<std::string, seal::Plaintext> &encoded_inputs,
  std::unordered_map<std::string, seal::Ciphertext> &encrypted_outputs,
  std::unordered_map<std::string, seal::Plaintext> &encoded_outputs, const seal::BatchEncoder &encoder,
  const seal::Encryptor &encryptor, const seal::Evaluator &evaluator, const seal::RelinKeys &relin_keys,
  const seal::GaloisKeys &galois_keys)
{
  seal::Ciphertext c0 = encrypted_inputs.at("c0");
  seal::Ciphertext c1;
  evaluator.rotate_rows(c0, 1, galois_keys, c1);
  seal::Ciphertext c2;
  evaluator.add(c0, c1, c2);
  seal::Ciphertext c3;
  evaluator.rotate_rows(c2, 5, galois_keys, c3);
  seal::Ciphertext c4;
  evaluator.add(c2, c3, c4);
  encrypted_outputs.emplace("c6", std::move(c4));
}
