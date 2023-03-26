#include "gen_he_matrix_mul_16_16x16_16_log.hpp"
#include "utils.hpp"
#include <chrono>
#include <iostream>
#include <vector>

using namespace std;
using namespace seal;

int main()
{
  int m_a = 16;
  int n_a = 16;
  int m_b = 16;
  int n_b = 16;
  string dim_info = to_string(m_a) + "_" + to_string(n_a) + "x" + to_string(m_b) + "_" + to_string(n_b);

  string func_name = "matrix_mul_" + dim_info + "_log";

  clear_args_info_map clear_inputs, clear_outputs;
  parse_inputs_outputs_file("../" + func_name + "_rand_example.txt", clear_inputs, clear_outputs);

  SEALContext context = create_context();

  BatchEncoder batch_encoder(context);
  KeyGenerator keygen(context);
  const SecretKey &secret_key = keygen.secret_key();
  PublicKey public_key;
  keygen.create_public_key(public_key);
  RelinKeys relin_keys;
  keygen.create_relin_keys(relin_keys);
  GaloisKeys galois_keys;
  keygen.create_galois_keys(get_rotations_steps(), galois_keys);
  Encryptor encryptor(context, public_key);
  Evaluator evaluator(context);
  Decryptor decryptor(context, secret_key);

  encrypted_args_map encrypted_inputs;
  encoded_args_map encoded_inputs;
  prepare_he_inputs(batch_encoder, encryptor, clear_inputs, encrypted_inputs, encoded_inputs);

  auto &context_data = *context.first_context_data();

  encrypted_args_map encrypted_outputs;
  encoded_args_map encoded_outputs;

  matrix_mul_16_16x16_16_log(
    encrypted_inputs, encoded_inputs, encrypted_outputs, encoded_outputs, evaluator, batch_encoder, relin_keys,
    galois_keys, public_key);

  clear_args_info_map obtained_clear_outputs;
  get_clear_outputs(
    batch_encoder, decryptor, encrypted_outputs, encoded_outputs, clear_outputs, obtained_clear_outputs);

  if (clear_outputs != obtained_clear_outputs)
    throw logic_error("clear_outputs != obtained_clear_outputs");

  print_variables_values(obtained_clear_outputs);

  print_encrypted_outputs_info(context, decryptor, encrypted_outputs);

  // get peak memory from /proc
  getchar();

  int repeat = 1;

  chrono::high_resolution_clock::time_point time_start, time_end;
  time_start = chrono::high_resolution_clock::now();
  for (int i = 0; i < repeat; ++i)
    matrix_mul_16_16x16_16_log(
      encrypted_inputs, encoded_inputs, encrypted_outputs, encoded_outputs, evaluator, batch_encoder, relin_keys,
      galois_keys, public_key);
  time_end = chrono::high_resolution_clock::now();

  chrono::microseconds time_diff = chrono::duration_cast<chrono::microseconds>(time_end - time_start);
  cout << "exec_time: " << time_diff.count() / repeat << " µs"
       << "\n";
}
