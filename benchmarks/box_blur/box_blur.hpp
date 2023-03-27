#include "seal/seal.h"
#include <cstdint>
#include <unordered_map>
#include <vector>
seal::SEALContext create_context()
{
  seal::EncryptionParameters params(seal::scheme_type::bfv);
  params.set_poly_modulus_degree(2048);
  params.set_plain_modulus(seal::PlainModulus::Batching(2048, 17));
  params.set_coeff_modulus(seal::CoeffModulus::Create(2048, {27, 27}));
  seal::SEALContext context(params, false, seal::sec_level_type::tc128);
  return context;
}

void box_blur(
  std::unordered_map<std::string, seal::Ciphertext> &encrypted_inputs,
  std::unordered_map<std::string, seal::Plaintext> &encoded_inputs,
  std::unordered_map<std::string, seal::Ciphertext> &encrypted_outputs,
  std::unordered_map<std::string, seal::Plaintext> &encoded_outputs, const seal::SEALContext &context,
  const seal::RelinKeys &relin_keys, const seal::GaloisKeys &galois_keys, const seal::PublicKey &public_key)
{
  seal::Ciphertext c0 = encrypted_inputs["c0"];
  seal::Evaluator evaluator(context);
  seal::Ciphertext ciphertext14;
  evaluator.rotate_vector(c0, 1, galois_keys, ciphertext14);
  seal::Ciphertext ciphertext15;
  evaluator.add(c0, ciphertext14, ciphertext15);
  evaluator.rotate_vector_inplace(ciphertext15, 5, galois_keys);
  evaluator.add_inplace(ciphertext14, c0);
  evaluator.add_inplace(ciphertext14, ciphertext15);
  seal::Ciphertext output = ciphertext14;
  encrypted_outputs.insert({"output", output});
}
