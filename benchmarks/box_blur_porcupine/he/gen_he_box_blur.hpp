#include "seal/seal.h"
#include <cstdint>
#include <map>
#include <vector>

seal::SEALContext create_context()
{
  seal::EncryptionParameters params(seal::scheme_type::bfv);
  params.set_poly_modulus_degree(4096);
  params.set_plain_modulus(seal::PlainModulus::Batching(4096, 17));
  params.set_coeff_modulus(seal::CoeffModulus::Create(4096, {54, 55}));
  seal::SEALContext context(params, false, seal::sec_level_type::tc128);
  return context;
}

std::vector<int> get_rotations_steps()
{
  std::vector<int> steps = {1, 5};
  return steps;
}

void box_blur(
  std::map<std::string, seal::Ciphertext> &encrypted_inputs, std::map<std::string, seal::Plaintext> &encoded_inputs,
  std::map<std::string, seal::Ciphertext> &encrypted_outputs, std::map<std::string, seal::Plaintext> &encoded_outputs,
  const seal::BatchEncoder &encoder, const seal::Encryptor &encryptor, const seal::Evaluator &evaluator,
  const seal::RelinKeys &relin_keys, const seal::GaloisKeys &galois_keys)
{
  seal::Ciphertext ciphertext0 = encrypted_inputs["c0"];
  seal::Ciphertext ciphertext13;
  evaluator.rotate_rows(ciphertext0, 1, galois_keys, ciphertext13);
  seal::Ciphertext ciphertext14;
  evaluator.add(ciphertext0, ciphertext13, ciphertext14);
  seal::Ciphertext ciphertext5;
  evaluator.rotate_rows(ciphertext14, 5, galois_keys, ciphertext5);
  seal::Ciphertext ciphertext6;
  evaluator.add(ciphertext14, ciphertext5, ciphertext6);
  encrypted_outputs.insert({"c6", ciphertext6});
}
