#include "examples.h"

using namespace std;
using namespace seal;

void example_batch_encoder()
{
  // Init encryption parameters
  EncryptionParameters parms(scheme_type::bfv);
  size_t poly_modulus_degree = 8192;
  parms.set_poly_modulus_degree(poly_modulus_degree);
  parms.set_coeff_modulus(CoeffModulus::BFVDefault(poly_modulus_degree));
  parms.set_plain_modulus(PlainModulus::Batching(poly_modulus_degree, 20));
  // Create a context
  SEALContext context(parms);
  // Create a key generator and generate diffrent keys
  KeyGenerator keygen(context);
  SecretKey secret_key = keygen.secret_key();
  PublicKey public_key;
  keygen.create_public_key(public_key);
  RelinKeys relin_keys;
  keygen.create_relin_keys(relin_keys);
  // Create objects used to prepare data (encode, encrypt, decrypt, decode), evaluate
  BatchEncoder batch_encoder(context);
  Encryptor encryptor(context, public_key);
  Evaluator evaluator(context);
  Decryptor decryptor(context, secret_key);
  // Init clear data
  size_t slot_count = batch_encoder.slot_count();
  vector<uint64_t> pod_matrix(slot_count, 0ULL);
  // Encode
  Plaintext plain_matrix;
  batch_encoder.encode(pod_matrix, plain_matrix);
  // Encrypt
  Ciphertext encrypted_matrix;
  encryptor.encrypt(plain_matrix, encrypted_matrix);
  // Evaluate
  evaluator.add_plain_inplace(encrypted_matrix, plain_matrix);
  evaluator.square_inplace(encrypted_matrix);
  evaluator.relinearize_inplace(encrypted_matrix, relin_keys);
  // Decrypt
  Plaintext plain_result;
  decryptor.decrypt(encrypted_matrix, plain_result);
  // Decode
  batch_encoder.decode(plain_result, pod_result);
}
