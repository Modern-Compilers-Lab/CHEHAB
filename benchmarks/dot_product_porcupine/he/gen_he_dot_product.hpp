#include "seal/seal.h"
#include <cstdint>
#include <map>
#include <vector>

seal::SEALContext create_context()
{
  seal::EncryptionParameters params(seal::scheme_type::bfv);
  params.set_poly_modulus_degree(8192);
  params.set_plain_modulus(seal::PlainModulus::Batching(8192, 18));
  params.set_coeff_modulus(seal::CoeffModulus::Create(8192, {60, 60, 60}));
  seal::SEALContext context(params, false, seal::sec_level_type::tc128);
  return context;
}

std::vector<int> get_rotations_steps()
{
  std::vector<int> steps = {1, 2, 4};
  return steps;
}

void dot_product(
  std::map<std::string, seal::Ciphertext> &encrypted_inputs, std::map<std::string, seal::Plaintext> &encoded_inputs,
  std::map<std::string, seal::Ciphertext> &encrypted_outputs, std::map<std::string, seal::Plaintext> &encoded_outputs,
  const seal::BatchEncoder &encoder, const seal::Encryptor &encryptor, const seal::Evaluator &evaluator,
  const seal::RelinKeys &relin_keys, const seal::GaloisKeys &galois_keys)
{
  seal::Ciphertext ciphertext1 = encrypted_inputs["c00"];
  seal::Ciphertext ciphertext0 = encrypted_inputs["c0"];
  seal::Ciphertext ciphertext13;
  evaluator.multiply(ciphertext0, ciphertext1, ciphertext13);
  seal::Ciphertext ciphertext2;
  evaluator.relinearize(ciphertext13, relin_keys, ciphertext2);
  seal::Ciphertext ciphertext3;
  evaluator.rotate_rows(ciphertext2, 4, galois_keys, ciphertext3);
  seal::Ciphertext ciphertext4;
  evaluator.add(ciphertext2, ciphertext3, ciphertext4);
  seal::Ciphertext ciphertext5;
  evaluator.rotate_rows(ciphertext4, 1, galois_keys, ciphertext5);
  seal::Ciphertext ciphertext6;
  evaluator.add(ciphertext5, ciphertext4, ciphertext6);
  seal::Ciphertext ciphertext7;
  evaluator.rotate_rows(ciphertext6, 2, galois_keys, ciphertext7);
  seal::Ciphertext ciphertext8;
  evaluator.add(ciphertext7, ciphertext6, ciphertext8);
  encrypted_outputs.insert({"c_result", ciphertext8});
}
