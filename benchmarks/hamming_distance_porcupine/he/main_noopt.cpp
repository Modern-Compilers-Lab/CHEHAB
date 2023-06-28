#include <chrono>
#include <cstddef>
#include <fstream>
#include <iostream>
#include "gen_he_hamming_distance_noopt.hpp"
#include "utils.hpp"

using namespace std;
using namespace seal;

int main(int argc, char **argv)
{
  string app_name = "hamming_distance";
  ifstream is("../" + app_name + "_io_example.txt");
  if (!is)
    throw invalid_argument("failed to open file");

  EncryptionParameters params(scheme_type::bfv);
  size_t n = 4096;
  params.set_poly_modulus_degree(n);
  params.set_plain_modulus(PlainModulus::Batching(n, 20));
  ClearArgsInfo clear_inputs, clear_outputs;
  parse_inputs_outputs_file(params.plain_modulus(), is, clear_inputs, clear_outputs);
  params.set_coeff_modulus(CoeffModulus::Create(n, {36, 36, 37}));
  SEALContext context(params, false, sec_level_type::tc128);
  BatchEncoder batch_encoder(context);
  KeyGenerator keygen(context);
  const SecretKey &secret_key = keygen.secret_key();
  PublicKey public_key;
  keygen.create_public_key(public_key);
  RelinKeys relin_keys;
  keygen.create_relin_keys(relin_keys);
  GaloisKeys galois_keys;
  keygen.create_galois_keys(get_rotation_steps_hamming_distance_noopt(), galois_keys);
  Encryptor encryptor(context, public_key);
  Evaluator evaluator(context);
  Decryptor decryptor(context, secret_key);

  EncryptedArgs encrypted_inputs;
  EncodedArgs encoded_inputs;
  prepare_he_inputs(batch_encoder, encryptor, clear_inputs, encrypted_inputs, encoded_inputs);
  EncryptedArgs encrypted_outputs;
  EncodedArgs encoded_outputs;

  chrono::high_resolution_clock::time_point time_start, time_end;
  chrono::duration<double, milli> time_sum(0);
  time_start = chrono::high_resolution_clock::now();
  hamming_distance_noopt(
    encrypted_inputs, encoded_inputs, encrypted_outputs, encoded_outputs, batch_encoder, encryptor, evaluator,
    relin_keys, galois_keys);
  time_end = chrono::high_resolution_clock::now();
  time_sum = time_end - time_start;

  ClearArgsInfo obtained_clear_outputs;
  get_clear_outputs(
    batch_encoder, decryptor, encrypted_outputs, encoded_outputs, clear_outputs, obtained_clear_outputs);
  if (clear_outputs != obtained_clear_outputs)
    throw logic_error("clear_outputs != obtained_clear_outputs");

  cout << time_sum.count() << '\n';
}
