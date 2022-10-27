#include <bits/stdc++.h>
#include <seal/seal.h>

using namespace std;
using namespace seal;

using EncryptedInputs = std::unordered_map<std::string, seal::Ciphertext>;
using EncodedInputs = std::unordered_map<std::string, seal::Plaintext>;
using EncodedOutputs = EncodedInputs;
using EncryptedOutputs = EncryptedInputs;
using Context = seal::SEALContext;

void test1(
  EncryptedInputs &cipher_inputs, EncodedInputs &plain_inputs, EncodedOutputs &plain_outputs,
  EncryptedOutputs &cipher_outputs, const Context &context, const RelinKeys &relin_keys, const GaloisKeys &galois_keys,
  const PublicKey &public_key)
{
  Evaluator evaluator(context);
  Plaintext pt1 = plain_inputs["pt1"];
  Ciphertext ct1 = cipher_inputs["ct1"];
  BatchEncoder encoder(context);
  std::vector<uint64_t> scalar0_clear = {2};
  Plaintext scalar0;
  encoder.encode(scalar0_clear, scalar0);
  Ciphertext ciphertext2;
  evaluator.add_plain(ct1, pt1, ciphertext2);
  Ciphertext ciphertext3;
  evaluator.multiply_plain(ciphertext2, scalar0, ciphertext3);
  Ciphertext ciphertext4;
  evaluator.add(ciphertext3, ct1, ciphertext4);
  Ciphertext ciphertext5;
  evaluator.add(ciphertext4, ct1, ciphertext5);
  Ciphertext ciphertext6;
  evaluator.add_plain(ciphertext5, pt1, ciphertext6);
  Ciphertext output1 = ciphertext6;
  cipher_outputs.insert({"output1", output1});
}

void print_vector(const std::vector<uint64_t> &v)
{
  for (auto &x : v)
    std::cout << x << " ";
  std::cout << '\n';
}

int main(void)
{

  EncryptionParameters params(scheme_type::bfv);
  size_t polynomial_modulus_degree = 2 << 13;
  params.set_poly_modulus_degree(polynomial_modulus_degree);
  params.set_coeff_modulus(CoeffModulus::BFVDefault(polynomial_modulus_degree));
  params.set_plain_modulus(PlainModulus::Batching(polynomial_modulus_degree, 20));
  SEALContext context(params);

  seal::KeyGenerator keygen(context);

  EncryptedInputs encrypted_inputs;
  EncodedInputs encoded_inputs;
  EncodedOutputs encoded_outputs;
  EncryptedOutputs encrypted_outputs;

  BatchEncoder encoder(context);
  Plaintext pt1;
  encoder.encode(std::vector<uint64_t>{1, 3, 4}, pt1);

  SecretKey secret_key = keygen.secret_key();
  PublicKey public_key;
  keygen.create_public_key(public_key);
  seal::Encryptor encryptor(context, public_key);
  Ciphertext ct1;
  encryptor.encrypt(pt1, ct1);

  encoded_inputs["pt1"] = pt1;
  encrypted_inputs["ct1"] = ct1;

  RelinKeys relin_keys;
  GaloisKeys galois_keys;

  // server

  test1(
    encrypted_inputs, encoded_inputs, encoded_outputs, encrypted_outputs, context, relin_keys, galois_keys, public_key);

  // client
  Ciphertext output1 = encrypted_outputs["output1"];

  Plaintext output1_pt;

  seal::Decryptor decryptor(context, secret_key);

  std::cout << decryptor.invariant_noise_budget(output1) << '\n';
  decryptor.decrypt(output1, output1_pt);

  std::vector<uint64_t> clear_data;

  encoder.decode(output1_pt, clear_data);

  print_vector(clear_data);

  std::cout << '\n';
  return 0;
}
