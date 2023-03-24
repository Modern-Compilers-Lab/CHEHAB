#include "seal/seal.h"
#include <cstdint>
#include <unordered_map>
#include <vector>

seal::SEALContext create_context()
{
  seal::EncryptionParameters params(seal::scheme_type::bfv);
  params.set_poly_modulus_degree(4096);
  params.set_plain_modulus(seal::PlainModulus::Batching(4096, 18));
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
  std::unordered_map<std::string, seal::Ciphertext> &encrypted_inputs,
  std::unordered_map<std::string, seal::Plaintext> &encoded_inputs,
  std::unordered_map<std::string, seal::Ciphertext> &encrypted_outputs,
  std::unordered_map<std::string, seal::Plaintext> &encoded_outputs, const seal::Evaluator &evaluator,
  const seal::RelinKeys &relin_keys, const seal::GaloisKeys &galois_keys, const seal::PublicKey &public_key)
{
  seal::Ciphertext c0 = encrypted_inputs["c0"];
  seal::Ciphertext ciphertext14;
  evaluator.rotate_rows(c0, 1, galois_keys, ciphertext14);
  seal::Ciphertext ciphertext15;
  evaluator.add(c0, ciphertext14, ciphertext15);
  seal::Ciphertext ciphertext5;
  evaluator.rotate_rows(ciphertext15, 5, galois_keys, ciphertext5);
  seal::Ciphertext ciphertext6;
  evaluator.add(ciphertext15, ciphertext5, ciphertext6);
  seal::Ciphertext output = ciphertext6;
  encrypted_outputs.insert({"output", output});
}
