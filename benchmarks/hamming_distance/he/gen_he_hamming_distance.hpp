#include "seal/seal.h"
#include <cstdint>
#include <map>
#include <vector>

seal::SEALContext create_context()
{
  seal::EncryptionParameters params(seal::scheme_type::bfv);
  params.set_poly_modulus_degree(8192);
  params.set_plain_modulus(seal::PlainModulus::Batching(8192, 17));
  params.set_coeff_modulus(seal::CoeffModulus::Create(8192, {55, 54, 54, 55}));
  seal::SEALContext context(params, true, seal::sec_level_type::tc128);
  return context;
}

std::vector<int> get_rotations_steps()
{
  std::vector<int> steps = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512};
  return steps;
}

void hamming_distance(
  std::map<std::string, seal::Ciphertext> &encrypted_inputs, std::map<std::string, seal::Plaintext> &encoded_inputs,
  std::map<std::string, seal::Ciphertext> &encrypted_outputs, std::map<std::string, seal::Plaintext> &encoded_outputs,
  const seal::BatchEncoder &encoder, const seal::Encryptor &encryptor, const seal::Evaluator &evaluator,
  const seal::RelinKeys &relin_keys, const seal::GaloisKeys &galois_keys)
{
  seal::Ciphertext ciphertext1 = encrypted_inputs["c2"];
  seal::Ciphertext ciphertext0 = encrypted_inputs["c1"];
  seal::Ciphertext ciphertext38;
  evaluator.multiply(ciphertext0, ciphertext1, ciphertext38);
  seal::Ciphertext ciphertext3;
  evaluator.relinearize(ciphertext38, relin_keys, ciphertext3);
  std::vector<std::int64_t> scalar0_clear(8192, 2);
  seal::Plaintext scalar0;
  encoder.encode(scalar0_clear, scalar0);
  seal::Ciphertext ciphertext4;
  evaluator.multiply_plain(ciphertext3, scalar0, ciphertext4);
  seal::Ciphertext ciphertext2;
  evaluator.add(ciphertext0, ciphertext1, ciphertext2);
  seal::Ciphertext ciphertext5;
  evaluator.sub(ciphertext2, ciphertext4, ciphertext5);
  seal::Ciphertext ciphertext6;
  evaluator.rotate_rows(ciphertext5, 512, galois_keys, ciphertext6);
  seal::Ciphertext ciphertext6_level1;
  evaluator.mod_switch_to_next(ciphertext6, ciphertext6_level1);
  seal::Ciphertext ciphertext5_level1;
  evaluator.mod_switch_to_next(ciphertext5, ciphertext5_level1);
  seal::Ciphertext ciphertext7;
  evaluator.add(ciphertext5_level1, ciphertext6_level1, ciphertext7);
  seal::Ciphertext ciphertext8;
  evaluator.rotate_rows(ciphertext7, 256, galois_keys, ciphertext8);
  seal::Ciphertext ciphertext9;
  evaluator.add(ciphertext7, ciphertext8, ciphertext9);
  seal::Ciphertext ciphertext10;
  evaluator.rotate_rows(ciphertext9, 128, galois_keys, ciphertext10);
  seal::Ciphertext ciphertext11;
  evaluator.add(ciphertext9, ciphertext10, ciphertext11);
  seal::Ciphertext ciphertext12;
  evaluator.rotate_rows(ciphertext11, 64, galois_keys, ciphertext12);
  seal::Ciphertext ciphertext13;
  evaluator.add(ciphertext11, ciphertext12, ciphertext13);
  seal::Ciphertext ciphertext14;
  evaluator.rotate_rows(ciphertext13, 32, galois_keys, ciphertext14);
  seal::Ciphertext ciphertext15;
  evaluator.add(ciphertext13, ciphertext14, ciphertext15);
  seal::Ciphertext ciphertext16;
  evaluator.rotate_rows(ciphertext15, 16, galois_keys, ciphertext16);
  seal::Ciphertext ciphertext17;
  evaluator.add(ciphertext15, ciphertext16, ciphertext17);
  seal::Ciphertext ciphertext18;
  evaluator.rotate_rows(ciphertext17, 8, galois_keys, ciphertext18);
  seal::Ciphertext ciphertext19;
  evaluator.add(ciphertext17, ciphertext18, ciphertext19);
  seal::Ciphertext ciphertext20;
  evaluator.rotate_rows(ciphertext19, 4, galois_keys, ciphertext20);
  seal::Ciphertext ciphertext21;
  evaluator.add(ciphertext19, ciphertext20, ciphertext21);
  seal::Ciphertext ciphertext22;
  evaluator.rotate_rows(ciphertext21, 2, galois_keys, ciphertext22);
  seal::Ciphertext ciphertext23;
  evaluator.add(ciphertext21, ciphertext22, ciphertext23);
  seal::Ciphertext ciphertext24;
  evaluator.rotate_rows(ciphertext23, 1, galois_keys, ciphertext24);
  seal::Ciphertext ciphertext24_level0;
  evaluator.mod_switch_to_next(ciphertext24, ciphertext24_level0);
  seal::Ciphertext ciphertext23_level0;
  evaluator.mod_switch_to_next(ciphertext23, ciphertext23_level0);
  seal::Ciphertext ciphertext25;
  evaluator.add(ciphertext23_level0, ciphertext24_level0, ciphertext25);
  encrypted_outputs.insert({"result", ciphertext25});
}
