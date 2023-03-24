#include "gen_he_box_blur.hpp"
#include "utils.hpp"
#include <chrono>
#include <iostream>
#include <vector>

using namespace std;
using namespace seal;

int main()
{
  string func_name = "box_blur";

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

  int repeat = 1000;

  box_blur(
    encrypted_inputs, encoded_inputs, encrypted_outputs, encoded_outputs, evaluator, relin_keys, galois_keys,
    public_key);

  clear_args_info_map obtained_clear_outputs;
  get_clear_outputs(
    batch_encoder, decryptor, encrypted_outputs, encoded_outputs, clear_outputs, obtained_clear_outputs);

  if (clear_outputs != obtained_clear_outputs)
    throw logic_error("clear_outputs != obtained_clear_outputs");

  get_encrypted_outputs_info(context, decryptor, encrypted_outputs);

  chrono::high_resolution_clock::time_point time_start, time_end;
  time_start = chrono::high_resolution_clock::now();
  for (int i = 0; i < repeat; ++i)
    box_blur(
      encrypted_inputs, encoded_inputs, encrypted_outputs, encoded_outputs, evaluator, relin_keys, galois_keys,
      public_key);
  time_end = chrono::high_resolution_clock::now();

  chrono::microseconds time_diff = chrono::duration_cast<chrono::microseconds>(time_end - time_start);
  cout << endl;
  cout << "exec_time: " << time_diff.count() / repeat << " µs" << endl;

  char c;
  cin >> c;
}
