#include "seal/seal.h"
#include <cstdint>
#include <unordered_map>
#include <vector>

seal::SEALContext create_context()
{
  seal::EncryptionParameters params(seal::scheme_type::bfv);
  params.set_poly_modulus_degree(4096);
  params.set_plain_modulus(seal::PlainModulus::Batching(4096, 18));
  params.set_coeff_modulus(seal::CoeffModulus::Create(4096, {54, 55}));
  seal::SEALContext context(params, false, seal::sec_level_type::tc128);
  return context;
}

std::vector<int> get_rotations_steps()
{
  std::vector<int> steps = {1, 2, 4, 5};
  return steps;
}

void gx_kernel(
  std::unordered_map<std::string, seal::Ciphertext> &encrypted_inputs,
  std::unordered_map<std::string, seal::Plaintext> &encoded_inputs,
  std::unordered_map<std::string, seal::Ciphertext> &encrypted_outputs,
  std::unordered_map<std::string, seal::Plaintext> &encoded_outputs, const seal::Evaluator &evaluator,
  const seal::RelinKeys &relin_keys, const seal::GaloisKeys &galois_keys, const seal::PublicKey &public_key)
{
  seal::Ciphertext c0 = encrypted_inputs["c0"];
  seal::Ciphertext ciphertext23;
  evaluator.rotate_rows(c0, 2, galois_keys, ciphertext23);
  seal::Ciphertext ciphertext24;
  evaluator.add(ciphertext23, c0, ciphertext24);
  seal::Ciphertext ciphertext26;
  evaluator.add(ciphertext24, ciphertext24, ciphertext26);
  seal::Ciphertext ciphertext45;
  evaluator.rotate_rows(ciphertext26, 1, galois_keys, ciphertext45);
  seal::Ciphertext ciphertext30;
  evaluator.rotate_rows(c0, 1, galois_keys, ciphertext30);
  seal::Ciphertext ciphertext31;
  evaluator.add(c0, ciphertext30, ciphertext31);
  seal::Ciphertext ciphertext40;
  evaluator.rotate_rows(ciphertext31, 5, galois_keys, ciphertext40);
  seal::Ciphertext ciphertext35;
  evaluator.rotate_rows(c0, 4, galois_keys, ciphertext35);
  seal::Ciphertext ciphertext36;
  evaluator.add(c0, ciphertext35, ciphertext36);
  seal::Ciphertext ciphertext41;
  evaluator.add(ciphertext36, ciphertext40, ciphertext41);
  seal::Ciphertext ciphertext46;
  evaluator.add(ciphertext41, ciphertext45, ciphertext46);
  seal::Ciphertext ciphertext12;
  evaluator.rotate_rows(ciphertext46, 1, galois_keys, ciphertext12);
  seal::Ciphertext output = ciphertext12;
  encrypted_outputs.insert({"output", output});
}
