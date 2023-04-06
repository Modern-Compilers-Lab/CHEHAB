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
  seal::SEALContext context(params, true, seal::sec_level_type::tc128);
  return context;
}

std::vector<int> get_rotations_steps()
{
  std::vector<int> steps = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512};
  return steps;
}

void dot_product(
  std::map<std::string, seal::Ciphertext> &encrypted_inputs, std::map<std::string, seal::Plaintext> &encoded_inputs,
  std::map<std::string, seal::Ciphertext> &encrypted_outputs, std::map<std::string, seal::Plaintext> &encoded_outputs,
  const seal::BatchEncoder &encoder, const seal::Encryptor &encryptor, const seal::Evaluator &evaluator,
  const seal::RelinKeys &relin_keys, const seal::GaloisKeys &galois_keys)
{
  seal::Ciphertext ciphertext1 = encrypted_inputs["c1"];
  seal::Ciphertext ciphertext0 = encrypted_inputs["c0"];
  seal::Ciphertext ciphertext34;
  evaluator.multiply(ciphertext0, ciphertext1, ciphertext34);
  seal::Ciphertext ciphertext2;
  evaluator.relinearize(ciphertext34, relin_keys, ciphertext2);
  seal::Ciphertext ciphertext3;
  evaluator.rotate_rows(ciphertext2, 512, galois_keys, ciphertext3);
  seal::Ciphertext ciphertext4;
  evaluator.add(ciphertext2, ciphertext3, ciphertext4);
  seal::Ciphertext ciphertext5;
  evaluator.rotate_rows(ciphertext4, 256, galois_keys, ciphertext5);
  seal::Ciphertext ciphertext6;
  evaluator.add(ciphertext4, ciphertext5, ciphertext6);
  seal::Ciphertext ciphertext7;
  evaluator.rotate_rows(ciphertext6, 128, galois_keys, ciphertext7);
  seal::Ciphertext ciphertext8;
  evaluator.add(ciphertext6, ciphertext7, ciphertext8);
  seal::Ciphertext ciphertext9;
  evaluator.rotate_rows(ciphertext8, 64, galois_keys, ciphertext9);
  seal::Ciphertext ciphertext10;
  evaluator.add(ciphertext8, ciphertext9, ciphertext10);
  seal::Ciphertext ciphertext11;
  evaluator.rotate_rows(ciphertext10, 32, galois_keys, ciphertext11);
  seal::Ciphertext ciphertext12;
  evaluator.add(ciphertext10, ciphertext11, ciphertext12);
  seal::Ciphertext ciphertext12_level0;
  evaluator.mod_switch_to_next(ciphertext12, ciphertext12_level0);
  seal::Ciphertext ciphertext13;
  evaluator.rotate_rows(ciphertext12_level0, 16, galois_keys, ciphertext13);
  seal::Ciphertext ciphertext14;
  evaluator.add(ciphertext12_level0, ciphertext13, ciphertext14);
  seal::Ciphertext ciphertext15;
  evaluator.rotate_rows(ciphertext14, 8, galois_keys, ciphertext15);
  seal::Ciphertext ciphertext16;
  evaluator.add(ciphertext14, ciphertext15, ciphertext16);
  seal::Ciphertext ciphertext17;
  evaluator.rotate_rows(ciphertext16, 4, galois_keys, ciphertext17);
  seal::Ciphertext ciphertext18;
  evaluator.add(ciphertext16, ciphertext17, ciphertext18);
  seal::Ciphertext ciphertext19;
  evaluator.rotate_rows(ciphertext18, 2, galois_keys, ciphertext19);
  seal::Ciphertext ciphertext20;
  evaluator.add(ciphertext18, ciphertext19, ciphertext20);
  seal::Ciphertext ciphertext21;
  evaluator.rotate_rows(ciphertext20, 1, galois_keys, ciphertext21);
  seal::Ciphertext ciphertext22;
  evaluator.add(ciphertext20, ciphertext21, ciphertext22);
  encrypted_outputs.insert({"result", ciphertext22});
}
