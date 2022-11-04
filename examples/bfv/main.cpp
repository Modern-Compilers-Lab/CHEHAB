#include "generated_defs_example0.hpp"

using namespace std;

int main()
{
  ufhe::Config::set_backend(ufhe::api::backend_type::seal);
  // Parameters
  ufhe::Scheme scheme(ufhe::api::scheme_type::bfv);
  ufhe::EncryptionParams params(scheme);
  size_t poly_modulus_degree = 8192;
  params.set_poly_modulus_degree(poly_modulus_degree);
  ufhe::CoeffModulus coeff_modulus({8796092858369, 8796092792833, 17592186028033, 17592185438209, 17592184717313});
  params.set_coeff_modulus(coeff_modulus);
  params.set_plain_modulus(ufhe::Modulus(1032193));
  cout << "plain_modulus: " << params.plain_modulus().value() << endl;
  cout << "coeff_modulus: " << endl;
  for (const ufhe::api::Modulus &e : params.coeff_modulus().value())
    cout << e.value() << " ";
  cout << endl;
  ufhe::EncryptionContext context(params);

  // Input preparation
  ufhe::BatchEncoder batch_encoder(context);
  size_t slot_count = batch_encoder.slot_count();
  cout << "slot_count " << slot_count << endl;
  vector<int64_t> a_clear(slot_count, 3);
  vector<int64_t> b_clear(slot_count, -1);

  // Encode
  ufhe::Plaintext a_plain;
  batch_encoder.encode(a_clear, a_plain);
  ufhe::Plaintext b_plain;
  batch_encoder.encode(b_clear, b_plain);

  // Encrypt
  ufhe::KeyGenerator keygen(context);
  ufhe::PublicKey pk;
  keygen.create_public_key(pk);
  ufhe::Encryptor encryptor(context, pk);
  ufhe::Ciphertext a_encrypted;
  encryptor.encrypt(a_plain, a_encrypted);
  ufhe::Ciphertext b_encrypted;
  encryptor.encrypt(b_plain, b_encrypted);

  // Evaluate
  ufhe::RelinKeys relinkeys;
  keygen.create_relin_keys(relinkeys);
  ufhe::GaloisKeys galoiskeys;
  keygen.create_galois_keys(galoiskeys);

  EncryptedInputs inputs;
  inputs["a"] = a_encrypted;
  inputs["b"] = b_encrypted;
  EncryptedOutputs outputs;
  example0(context, inputs, relinkeys, galoiskeys, outputs);
  ufhe::Ciphertext &r_encrypted = outputs["r"];

  // Decrypt
  const ufhe::SecretKey &sk = keygen.secret_key();
  ufhe::Decryptor decryptor(context, sk);
  ufhe::Plaintext r_plain;
  decryptor.decrypt(r_encrypted, r_plain);

  // Decode
  vector<int64_t> r_clear;
  batch_encoder.decode(r_plain, r_clear);

  // Show results
  print_matrix(r_clear, slot_count / 2);
  return 0;
}
