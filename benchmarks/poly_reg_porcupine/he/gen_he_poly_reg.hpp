#include <cstdint>
#include <unordered_map>
#include <utility>
#include <vector>
#include "seal/seal.h"

seal::SEALContext create_context()
{
  seal::EncryptionParameters params(seal::scheme_type::bfv);
  params.set_poly_modulus_degree(8192);
  params.set_plain_modulus(seal::PlainModulus::Batching(8192, 20));
  params.set_coeff_modulus(seal::CoeffModulus::Create(8192, {60, 60, 60}));
  seal::SEALContext context(params, true, seal::sec_level_type::tc128);
  return context;
}

void poly_reg_baseline(
  const std::unordered_map<std::string, seal::Ciphertext> &encrypted_inputs,
  const std::unordered_map<std::string, seal::Plaintext> &encoded_inputs,
  std::unordered_map<std::string, seal::Ciphertext> &encrypted_outputs,
  std::unordered_map<std::string, seal::Plaintext> &encoded_outputs, const seal::BatchEncoder &encoder,
  const seal::Encryptor &encryptor, const seal::Evaluator &evaluator, const seal::RelinKeys &relin_keys,
  const seal::GaloisKeys &galois_keys)
{
  seal::Ciphertext c0 = encrypted_inputs.at("c0");
  seal::Ciphertext c1 = encrypted_inputs.at("c1");
  seal::Ciphertext c2 = encrypted_inputs.at("c2");
  seal::Ciphertext c3 = encrypted_inputs.at("c3");
  seal::Ciphertext c4 = encrypted_inputs.at("c4");
  seal::Ciphertext c5;
  evaluator.square(c0, c5);
  evaluator.relinearize_inplace(c5, relin_keys);
  seal::Ciphertext c6;
  evaluator.multiply(c5, c4, c6);
  evaluator.relinearize_inplace(c6, relin_keys);
  seal::Ciphertext c7;
  evaluator.multiply(c0, c3, c7);
  evaluator.relinearize_inplace(c7, relin_keys);
  seal::Ciphertext c8;
  evaluator.add(c6, c7, c8);
  seal::Ciphertext c9;
  evaluator.add(c8, c2, c9);
  seal::Ciphertext c10;
  evaluator.sub(c1, c9, c10);
  encrypted_outputs.emplace("c_result", std::move(c10));
}

void poly_reg_opt1(
  const std::unordered_map<std::string, seal::Ciphertext> &encrypted_inputs,
  const std::unordered_map<std::string, seal::Plaintext> &encoded_inputs,
  std::unordered_map<std::string, seal::Ciphertext> &encrypted_outputs,
  std::unordered_map<std::string, seal::Plaintext> &encoded_outputs, const seal::BatchEncoder &encoder,
  const seal::Encryptor &encryptor, const seal::Evaluator &evaluator, const seal::RelinKeys &relin_keys,
  const seal::GaloisKeys &galois_keys)
{
  seal::Ciphertext c0 = encrypted_inputs.at("c0");
  seal::Ciphertext c1 = encrypted_inputs.at("c1");
  seal::Ciphertext c2 = encrypted_inputs.at("c2");
  seal::Ciphertext c3 = encrypted_inputs.at("c3");
  seal::Ciphertext c4 = encrypted_inputs.at("c4");
  seal::Ciphertext c5;
  evaluator.multiply(c0, c4, c5);
  evaluator.relinearize_inplace(c5, relin_keys);
  seal::Ciphertext c6;
  evaluator.add(c5, c3, c6);
  seal::Ciphertext c7;
  evaluator.multiply(c0, c6, c7);
  evaluator.relinearize_inplace(c7, relin_keys);
  seal::Ciphertext c8;
  evaluator.add(c7, c2, c8);
  seal::Ciphertext c9;
  evaluator.sub(c1, c8, c9);
  encrypted_outputs.emplace("c_result", std::move(c9));
}
