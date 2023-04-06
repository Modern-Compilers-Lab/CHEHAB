#include "seal/seal.h"
#include <cstdint>
#include <map>
#include <vector>

seal::SEALContext create_context()
{
  seal::EncryptionParameters params(seal::scheme_type::bfv);
  params.set_poly_modulus_degree(8192);
  params.set_plain_modulus(seal::PlainModulus::Batching(8192, 22));
  params.set_coeff_modulus(seal::CoeffModulus::Create(8192, {60, 60, 60}));
  seal::SEALContext context(params, false, seal::sec_level_type::tc128);
  return context;
}

std::vector<int> get_rotations_steps()
{
  std::vector<int> steps = {4, 1018, 1019};
  return steps;
}

void roberts_cross(
  std::map<std::string, seal::Ciphertext> &encrypted_inputs, std::map<std::string, seal::Plaintext> &encoded_inputs,
  std::map<std::string, seal::Ciphertext> &encrypted_outputs, std::map<std::string, seal::Plaintext> &encoded_outputs,
  const seal::BatchEncoder &encoder, const seal::Encryptor &encryptor, const seal::Evaluator &evaluator,
  const seal::RelinKeys &relin_keys, const seal::GaloisKeys &galois_keys)
{
  seal::Ciphertext ciphertext0 = encrypted_inputs["c0"];
  seal::Ciphertext ciphertext5;
  evaluator.rotate_rows(ciphertext0, 1018, galois_keys, ciphertext5);
  seal::Ciphertext ciphertext6;
  evaluator.sub(ciphertext0, ciphertext5, ciphertext6);
  seal::Ciphertext ciphertext20;
  evaluator.multiply(ciphertext6, ciphertext6, ciphertext20);
  seal::Ciphertext ciphertext7;
  evaluator.relinearize(ciphertext20, relin_keys, ciphertext7);
  seal::Ciphertext ciphertext15;
  evaluator.rotate_rows(ciphertext0, 4, galois_keys, ciphertext15);
  seal::Ciphertext ciphertext16;
  evaluator.sub(ciphertext0, ciphertext15, ciphertext16);
  seal::Ciphertext ciphertext22;
  evaluator.multiply(ciphertext16, ciphertext16, ciphertext22);
  seal::Ciphertext ciphertext18;
  evaluator.relinearize(ciphertext22, relin_keys, ciphertext18);
  seal::Ciphertext ciphertext4;
  evaluator.rotate_rows(ciphertext18, 1019, galois_keys, ciphertext4);
  seal::Ciphertext ciphertext8;
  evaluator.add(ciphertext4, ciphertext7, ciphertext8);
  encrypted_outputs.insert({"c_result", ciphertext8});
}
