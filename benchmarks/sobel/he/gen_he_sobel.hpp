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
  seal::SEALContext context(params, true, seal::sec_level_type::tc128);
  return context;
}

std::vector<int> get_rotations_steps()
{
  std::vector<int> steps = {1, 32, 33, 992, 993, 1023, 1055, 2015};
  return steps;
}

void sobel(
  std::map<std::string, seal::Ciphertext> &encrypted_inputs, std::map<std::string, seal::Plaintext> &encoded_inputs,
  std::map<std::string, seal::Ciphertext> &encrypted_outputs, std::map<std::string, seal::Plaintext> &encoded_outputs,
  const seal::BatchEncoder &encoder, const seal::Encryptor &encryptor, const seal::Evaluator &evaluator,
  const seal::RelinKeys &relin_keys, const seal::GaloisKeys &galois_keys)
{
  seal::Ciphertext ciphertext0 = encrypted_inputs["img"];
  seal::Ciphertext ciphertext27;
  evaluator.rotate_rows(ciphertext0, 33, galois_keys, ciphertext27);
  seal::Ciphertext ciphertext2;
  evaluator.rotate_rows(ciphertext0, 32, galois_keys, ciphertext2);
  std::vector<std::int64_t> scalar3_clear(8192, 2);
  seal::Plaintext scalar3;
  encoder.encode(scalar3_clear, scalar3);
  seal::Ciphertext ciphertext25;
  evaluator.multiply_plain(ciphertext2, scalar3, ciphertext25);
  seal::Ciphertext ciphertext24;
  evaluator.rotate_rows(ciphertext0, 1055, galois_keys, ciphertext24);
  seal::Ciphertext ciphertext26;
  evaluator.add(ciphertext24, ciphertext25, ciphertext26);
  seal::Ciphertext ciphertext28;
  evaluator.add(ciphertext26, ciphertext27, ciphertext28);
  seal::Ciphertext ciphertext22;
  evaluator.rotate_rows(ciphertext0, 993, galois_keys, ciphertext22);
  seal::Ciphertext ciphertext1;
  evaluator.rotate_rows(ciphertext0, 992, galois_keys, ciphertext1);
  seal::Ciphertext ciphertext20;
  evaluator.multiply_plain(ciphertext1, scalar3, ciphertext20);
  seal::Ciphertext ciphertext18;
  evaluator.rotate_rows(ciphertext0, 2015, galois_keys, ciphertext18);
  seal::Ciphertext ciphertext19;
  evaluator.negate(ciphertext18, ciphertext19);
  seal::Ciphertext ciphertext21;
  evaluator.sub(ciphertext19, ciphertext20, ciphertext21);
  seal::Ciphertext ciphertext23;
  evaluator.sub(ciphertext21, ciphertext22, ciphertext23);
  seal::Ciphertext ciphertext29;
  evaluator.add(ciphertext23, ciphertext28, ciphertext29);
  seal::Ciphertext ciphertext62;
  evaluator.multiply(ciphertext29, ciphertext29, ciphertext62);
  seal::Ciphertext ciphertext62_level0;
  evaluator.mod_switch_to_next(ciphertext62, ciphertext62_level0);
  seal::Ciphertext ciphertext31;
  evaluator.relinearize(ciphertext62_level0, relin_keys, ciphertext31);
  seal::Ciphertext ciphertext13;
  evaluator.negate(ciphertext24, ciphertext13);
  seal::Ciphertext ciphertext15;
  evaluator.add(ciphertext13, ciphertext27, ciphertext15);
  seal::Ciphertext ciphertext9;
  evaluator.rotate_rows(ciphertext0, 1, galois_keys, ciphertext9);
  seal::Ciphertext ciphertext10;
  evaluator.multiply_plain(ciphertext9, scalar3, ciphertext10);
  seal::Ciphertext ciphertext7;
  evaluator.rotate_rows(ciphertext0, 1023, galois_keys, ciphertext7);
  std::vector<std::int64_t> scalar0_clear(8192, -2);
  seal::Plaintext scalar0;
  encoder.encode(scalar0_clear, scalar0);
  seal::Ciphertext ciphertext8;
  evaluator.multiply_plain(ciphertext7, scalar0, ciphertext8);
  seal::Ciphertext ciphertext11;
  evaluator.add(ciphertext8, ciphertext10, ciphertext11);
  seal::Ciphertext ciphertext6;
  evaluator.add(ciphertext19, ciphertext22, ciphertext6);
  seal::Ciphertext ciphertext16;
  evaluator.add(ciphertext6, ciphertext11, ciphertext16);
  seal::Ciphertext ciphertext17;
  evaluator.add(ciphertext16, ciphertext15, ciphertext17);
  seal::Ciphertext ciphertext64;
  evaluator.multiply(ciphertext17, ciphertext17, ciphertext64);
  seal::Ciphertext ciphertext64_level0;
  evaluator.mod_switch_to_next(ciphertext64, ciphertext64_level0);
  seal::Ciphertext ciphertext30;
  evaluator.relinearize(ciphertext64_level0, relin_keys, ciphertext30);
  seal::Ciphertext ciphertext32;
  evaluator.add(ciphertext30, ciphertext31, ciphertext32);
  encrypted_outputs.insert({"result", ciphertext32});
}
