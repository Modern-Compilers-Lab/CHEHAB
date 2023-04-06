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

std::vector<int> get_rotations_steps()
{
  std::vector<int> steps = {1, 31, 32, 33, 991, 992, 993, 1023};
  return steps;
}

void sharpening_filter(
  std::map<std::string, seal::Ciphertext> &encrypted_inputs, std::map<std::string, seal::Plaintext> &encoded_inputs,
  std::map<std::string, seal::Ciphertext> &encrypted_outputs, std::map<std::string, seal::Plaintext> &encoded_outputs,
  const seal::BatchEncoder &encoder, const seal::Encryptor &encryptor, const seal::Evaluator &evaluator,
  const seal::RelinKeys &relin_keys, const seal::GaloisKeys &galois_keys)
{
  seal::Ciphertext ciphertext0 = encrypted_inputs["img"];
  seal::Ciphertext ciphertext9;
  evaluator.rotate_rows(ciphertext0, 33, galois_keys, ciphertext9);
  seal::Ciphertext ciphertext8;
  evaluator.rotate_rows(ciphertext0, 32, galois_keys, ciphertext8);
  seal::Ciphertext ciphertext7;
  evaluator.rotate_rows(ciphertext0, 31, galois_keys, ciphertext7);
  seal::Ciphertext ciphertext6;
  evaluator.rotate_rows(ciphertext0, 1, galois_keys, ciphertext6);
  seal::Ciphertext ciphertext5;
  evaluator.rotate_rows(ciphertext0, 1023, galois_keys, ciphertext5);
  seal::Ciphertext ciphertext4;
  evaluator.rotate_rows(ciphertext0, 993, galois_keys, ciphertext4);
  seal::Ciphertext ciphertext3;
  evaluator.rotate_rows(ciphertext0, 992, galois_keys, ciphertext3);
  seal::Ciphertext ciphertext2;
  evaluator.rotate_rows(ciphertext0, 991, galois_keys, ciphertext2);
  std::vector<std::int64_t> scalar0_clear(8192, -8);
  seal::Plaintext scalar0;
  encoder.encode(scalar0_clear, scalar0);
  seal::Ciphertext ciphertext1;
  evaluator.multiply_plain(ciphertext0, scalar0, ciphertext1);
  seal::Ciphertext ciphertext11;
  evaluator.add(ciphertext1, ciphertext2, ciphertext11);
  seal::Ciphertext ciphertext12;
  evaluator.add(ciphertext11, ciphertext3, ciphertext12);
  seal::Ciphertext ciphertext13;
  evaluator.add(ciphertext12, ciphertext4, ciphertext13);
  seal::Ciphertext ciphertext14;
  evaluator.add(ciphertext13, ciphertext5, ciphertext14);
  seal::Ciphertext ciphertext15;
  evaluator.add(ciphertext14, ciphertext6, ciphertext15);
  seal::Ciphertext ciphertext16;
  evaluator.add(ciphertext15, ciphertext7, ciphertext16);
  seal::Ciphertext ciphertext17;
  evaluator.add(ciphertext16, ciphertext8, ciphertext17);
  seal::Ciphertext ciphertext18;
  evaluator.add(ciphertext17, ciphertext9, ciphertext18);
  std::vector<std::int64_t> scalar1_clear(8192, 2);
  seal::Plaintext scalar1;
  encoder.encode(scalar1_clear, scalar1);
  seal::Ciphertext ciphertext10;
  evaluator.multiply_plain(ciphertext0, scalar1, ciphertext10);
  seal::Ciphertext ciphertext19;
  evaluator.sub(ciphertext10, ciphertext18, ciphertext19);
  encrypted_outputs.insert({"result", ciphertext19});
}
