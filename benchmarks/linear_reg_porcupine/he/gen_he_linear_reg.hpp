#include "seal/seal.h"
#include <cstdint>
#include <map>
#include <vector>

seal::SEALContext create_context()
{
  seal::EncryptionParameters params(seal::scheme_type::bfv);
  params.set_poly_modulus_degree(8192);
  params.set_plain_modulus(seal::PlainModulus::Batching(8192, 18));
  params.set_coeff_modulus(seal::CoeffModulus::Create(8192, {60, 60}));
  seal::SEALContext context(params, false, seal::sec_level_type::tc128);
  return context;
}

void linear_reg(
  std::map<std::string, seal::Ciphertext> &encrypted_inputs, std::map<std::string, seal::Plaintext> &encoded_inputs,
  std::map<std::string, seal::Ciphertext> &encrypted_outputs, std::map<std::string, seal::Plaintext> &encoded_outputs,
  const seal::BatchEncoder &encoder, const seal::Encryptor &encryptor, const seal::Evaluator &evaluator,
  const seal::RelinKeys &relin_keys, const seal::GaloisKeys &galois_keys)
{
  seal::Ciphertext ciphertext3 = encrypted_inputs["c3"];
  seal::Ciphertext ciphertext0 = encrypted_inputs["c0"];
  seal::Ciphertext ciphertext2 = encrypted_inputs["c2"];
  seal::Ciphertext ciphertext8;
  evaluator.multiply(ciphertext2, ciphertext0, ciphertext8);
  seal::Ciphertext ciphertext4;
  evaluator.relinearize(ciphertext8, relin_keys, ciphertext4);
  seal::Ciphertext ciphertext1 = encrypted_inputs["c1"];
  seal::Ciphertext ciphertext5;
  evaluator.sub(ciphertext1, ciphertext4, ciphertext5);
  seal::Ciphertext ciphertext6;
  evaluator.sub(ciphertext5, ciphertext3, ciphertext6);
  encrypted_outputs.insert({"c_result", ciphertext6});
}
