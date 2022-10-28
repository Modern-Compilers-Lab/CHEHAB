#include "generated_defs_example0.hpp"
#include "ufhe.hpp"
#include <cstddef>
#include <iomanip>
#include <iostream>

using namespace ufhe::seal_backend;
using namespace std;

int main()
{
  // Parameters
  SchemeType scheme(1);
  EncryptionParameters params(scheme);
  size_t poly_modulus_degree = 8192;
  params.set_poly_modulus_degree(poly_modulus_degree);
  params.set_coeff_modulus(CoeffModulus::Default(poly_modulus_degree));
  params.set_plain_modulus(Modulus::PlainModulus(poly_modulus_degree, 20));
  cout << "plain_modulus: " << params.plain_modulus().value() << endl;
  cout << "coeff_modulus: " << endl;
  for (const ufhe::IModulus &e : params.coeff_modulus().value())
    cout << e.value() << " ";
  cout << endl;
  EncryptionContext context(params);
  // Input preparation
  BatchEncoder batch_encoder(context);
  size_t slot_count = batch_encoder.slot_count();
  cout << "slot_count " << slot_count << endl;
  vector<uint64_t> a_clear(slot_count);
  for (int i = 0; i < slot_count; i++)
    a_clear[i] = i;
  vector<uint64_t> b_clear(slot_count, 2);
  // Encode
  Plaintext a_plain;
  batch_encoder.encode(a_clear, a_plain);
  Plaintext b_plain;
  batch_encoder.encode(b_clear, b_plain);
  // Encrypt
  KeyGenerator keygen(context);
  PublicKey pk;
  keygen.create_public_key(pk);
  Encryptor encryptor(context, pk);
  Ciphertext a_encrypted;
  encryptor.encrypt(a_plain, a_encrypted);
  Ciphertext b_encrypted;
  encryptor.encrypt(b_plain, b_encrypted);
  // Evaluate
  Inputs inputs{};
  Outputs outputs{};
  inputs["a"] = make_unique<Ciphertext>(a_encrypted);
  inputs["b"] = make_unique<Ciphertext>(b_encrypted);
  outputs["r"] = make_unique<Ciphertext>();
  Evaluator evaluator(context);
  example0(evaluator, RelinKeys(), GaloisKeys(), inputs, outputs);
  Ciphertext &r_encrypted = dynamic_cast<Ciphertext &>(*outputs["r"]);
  // Decrypt
  const SecretKey &sk = dynamic_cast<const SecretKey &>(keygen.secret_key());
  Decryptor decryptor(context, sk);
  Plaintext r_plain;
  decryptor.decrypt(r_encrypted, r_plain);
  // Decode
  vector<uint64_t> r_clear;
  batch_encoder.decode(r_plain, r_clear);
  // Show results
  print_matrix(r_clear, slot_count / 2);
  return 0;
}
