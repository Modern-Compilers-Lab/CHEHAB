#include "../box_blur.hpp"
#include <iostream>
#include <vector>

using namespace std;
using namespace seal;

int main()
{
  SEALContext context = create_context();

  BatchEncoder batch_encoder(context);
  KeyGenerator keygen(context);
  const SecretKey &secret_key = keygen.secret_key();
  PublicKey public_key;
  keygen.create_public_key(public_key);
  RelinKeys relin_keys;
  keygen.create_relin_keys(relin_keys);
  GaloisKeys galois_keys;
  keygen.create_galois_keys(vector<int>{1, 5}, galois_keys);
  Encryptor encryptor(context, public_key);
  Evaluator evaluator(context);
  Decryptor decryptor(context, secret_key);

  auto &context_data = *context.first_context_data();
  int init_noise_budget =
    context_data.total_coeff_modulus_bit_count() - context_data.parms().plain_modulus().bit_count();

  size_t slot_count = batch_encoder.slot_count();
  vector<uint64_t> random_data(slot_count);
  random_device rd;
  for (size_t i = 0; i < slot_count; ++i)
    random_data[i] = context_data.parms().plain_modulus().reduce(rd());

  Plaintext c0_plain;
  batch_encoder.encode(random_data, c0_plain);
  Ciphertext c0_cipher;
  encryptor.encrypt(c0_plain, c0_cipher);

  unordered_map<string, Ciphertext> encrypted_inputs;
  encrypted_inputs["c0"] = c0_cipher;
  unordered_map<string, Plaintext> encoded_inputs;
  unordered_map<string, Ciphertext> encrypted_outputs;
  unordered_map<string, Plaintext> encoded_outputs;

  box_blur(
    encrypted_inputs, encoded_inputs, encrypted_outputs, encoded_outputs, evaluator, relin_keys, galois_keys,
    public_key);

  cout << decryptor.invariant_noise_budget(encrypted_outputs["output"]) << endl;
}
