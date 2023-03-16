#include "seal/seal.h"
#include <cstdint>
#include <unordered_map>
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

void sharpening_filter(
  std::unordered_map<std::string, seal::Ciphertext> &encrypted_inputs,
  std::unordered_map<std::string, seal::Plaintext> &encoded_inputs,
  std::unordered_map<std::string, seal::Ciphertext> &encrypted_outputs,
  std::unordered_map<std::string, seal::Plaintext> &encoded_outputs, const seal::SEALContext &context,
  const seal::RelinKeys &relin_keys, const seal::GaloisKeys &galois_keys, const seal::PublicKey &public_key)
{
  seal::Ciphertext img = encrypted_inputs["img"];
  seal::Evaluator evaluator(context);
  seal::Ciphertext ciphertext9;
  evaluator.rotate_vector(img, 4, galois_keys, ciphertext9);
  seal::Ciphertext ciphertext8;
  evaluator.rotate_vector(img, 3, galois_keys, ciphertext8);
  seal::Ciphertext ciphertext7;
  evaluator.rotate_vector(img, 2, galois_keys, ciphertext7);
  seal::Ciphertext ciphertext6;
  evaluator.rotate_vector(img, 1, galois_keys, ciphertext6);
  seal::Ciphertext ciphertext5;
  evaluator.rotate_vector(img, 1023, galois_keys, ciphertext5);
  seal::Ciphertext ciphertext4;
  evaluator.rotate_vector(img, 1022, galois_keys, ciphertext4);
  seal::Ciphertext ciphertext3;
  evaluator.rotate_vector(img, 1021, galois_keys, ciphertext3);
  seal::Ciphertext ciphertext2;
  evaluator.rotate_vector(img, 1020, galois_keys, ciphertext2);
  seal::BatchEncoder encoder(context);
  std::vector<std::int64_t> scalar0_clear(1024, -8);
  seal::Plaintext scalar0;
  encoder.encode(scalar0_clear, scalar0);
  seal::Ciphertext ciphertext1;
  evaluator.multiply_plain(img, scalar0, ciphertext1);
  evaluator.add_inplace(ciphertext1, ciphertext2);
  evaluator.add_inplace(ciphertext1, ciphertext3);
  evaluator.add_inplace(ciphertext1, ciphertext4);
  evaluator.add_inplace(ciphertext1, ciphertext5);
  evaluator.add_inplace(ciphertext1, ciphertext6);
  evaluator.add_inplace(ciphertext1, ciphertext7);
  evaluator.add_inplace(ciphertext1, ciphertext8);
  evaluator.add_inplace(ciphertext1, ciphertext9);
  std::vector<std::int64_t> scalar1_clear(1024, 2);
  seal::Plaintext scalar1;
  encoder.encode(scalar1_clear, scalar1);
  seal::Ciphertext ciphertext12;
  evaluator.multiply_plain(img, scalar1, ciphertext12);
  evaluator.sub_inplace(ciphertext12, ciphertext1);
  seal::Ciphertext output = ciphertext12;
  encrypted_outputs.insert({"output", output});
}
