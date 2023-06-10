#include <chrono>
#include <cstddef>
#include <iostream>
#include "gen_he_cryptonets.hpp"
#include "utils.hpp"

using namespace std;
using namespace seal;

int main(int argc, char **argv)
{
  int repeat = 30;
  if (argc > 1)
    repeat = stoi(argv[1]);

  if (repeat < 1)
    throw invalid_argument("invalid repeat");

  string func_name = "cryptonets";
  clear_args_info_map clear_inputs, clear_outputs;
  parse_inputs_outputs_file("../" + func_name + "_rand_example.txt", clear_inputs, clear_outputs);

  seal::EncryptionParameters params(seal::scheme_type::bfv);
  size_t n = 8192;
  params.set_poly_modulus_degree(n);
  params.set_plain_modulus(65537);
  params.set_coeff_modulus(CoeffModulus::BFVDefault(n));
  seal::SEALContext context(params, false, seal::sec_level_type::tc128);
  BatchEncoder batch_encoder(context);
  KeyGenerator keygen(context);
  const SecretKey &secret_key = keygen.secret_key();
  PublicKey public_key;
  keygen.create_public_key(public_key);
  RelinKeys relin_keys;
  keygen.create_relin_keys(relin_keys);
  GaloisKeys galois_keys;
  Encryptor encryptor(context, public_key);
  Evaluator evaluator(context);
  Decryptor decryptor(context, secret_key);

  encrypted_args_map encrypted_inputs;
  encoded_args_map encoded_inputs;
  prepare_he_inputs(batch_encoder, encryptor, clear_inputs, encrypted_inputs, encoded_inputs);
  encrypted_args_map encrypted_outputs;
  encoded_args_map encoded_outputs;

  chrono::high_resolution_clock::time_point time_start, time_end;
  chrono::duration<double, milli> time_sum(0);
  time_start = chrono::high_resolution_clock::now();
  cryptonets(
    encrypted_inputs, encoded_inputs, encrypted_outputs, encoded_outputs, batch_encoder, encryptor, evaluator,
    relin_keys, galois_keys);
  time_end = chrono::high_resolution_clock::now();
  time_sum += time_end - time_start;

  clear_args_info_map obtained_clear_outputs;
  get_clear_outputs(
    batch_encoder, decryptor, encrypted_outputs, encoded_outputs, clear_outputs, obtained_clear_outputs);
  print_encrypted_outputs_info(context, decryptor, encrypted_outputs);
  if (clear_outputs != obtained_clear_outputs)
    throw logic_error("clear_outputs != obtained_clear_outputs");

  // get peak memory from /proc
  // getchar();

  time_start = chrono::high_resolution_clock::now();

  for (size_t i = 0; i < repeat - 1; ++i)
    cryptonets(
      encrypted_inputs, encoded_inputs, encrypted_outputs, encoded_outputs, batch_encoder, encryptor, evaluator,
      relin_keys, galois_keys);

  time_end = chrono::high_resolution_clock::now();
  time_sum += time_end - time_start;

  cout << "time: " << time_sum.count() / repeat << " ms\n";
}
