#include "seal/seal.h"
#include <cstdint>
#include <map>
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

std::vector<int> get_rotations_steps()
{
  std::vector<int> steps = {1, 2, 4, 5, 1017};
  return steps;
}

void gx_kernel(
  std::map<std::string, seal::Ciphertext> &encrypted_inputs, std::map<std::string, seal::Plaintext> &encoded_inputs,
  std::map<std::string, seal::Ciphertext> &encrypted_outputs, std::map<std::string, seal::Plaintext> &encoded_outputs,
  const seal::BatchEncoder &encoder, const seal::Encryptor &encryptor, const seal::Evaluator &evaluator,
  const seal::RelinKeys &relin_keys, const seal::GaloisKeys &galois_keys)
{
  seal::Ciphertext ciphertext0 = encrypted_inputs["c0"];
  seal::Ciphertext ciphertext22;
  evaluator.rotate_rows(ciphertext0, 2, galois_keys, ciphertext22);
  seal::Ciphertext ciphertext23;
  evaluator.add(ciphertext22, ciphertext0, ciphertext23);
  seal::Ciphertext ciphertext25;
  evaluator.add(ciphertext23, ciphertext23, ciphertext25);
  seal::Ciphertext ciphertext44;
  evaluator.rotate_rows(ciphertext25, 1017, galois_keys, ciphertext44);
  seal::Ciphertext ciphertext29;
  evaluator.rotate_rows(ciphertext0, 1017, galois_keys, ciphertext29);
  seal::Ciphertext ciphertext30;
  evaluator.add(ciphertext0, ciphertext29, ciphertext30);
  seal::Ciphertext ciphertext39;
  evaluator.rotate_rows(ciphertext30, 5, galois_keys, ciphertext39);
  seal::Ciphertext ciphertext34;
  evaluator.rotate_rows(ciphertext0, 4, galois_keys, ciphertext34);
  seal::Ciphertext ciphertext35;
  evaluator.add(ciphertext0, ciphertext34, ciphertext35);
  seal::Ciphertext ciphertext40;
  evaluator.add(ciphertext35, ciphertext39, ciphertext40);
  seal::Ciphertext ciphertext45;
  evaluator.add(ciphertext40, ciphertext44, ciphertext45);
  seal::Ciphertext ciphertext12;
  evaluator.rotate_rows(ciphertext45, 1, galois_keys, ciphertext12);
  encrypted_outputs.insert({"c12", ciphertext12});
}
