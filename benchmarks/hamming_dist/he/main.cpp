#include <chrono>
#include <cstddef>
#include <fstream>
#include <iostream>
#include <ostream>
#include "_gen_he_hamming_dist.hpp"
#include "utils.hpp"

using namespace std;
using namespace seal;

void print_bool_arg(bool arg, const string &name, ostream &os)
{
  os << (arg ? name : "no" + name);
}

int main(int argc, char **argv)
{
  bool opt = true;
  if (argc > 1)
    opt = stoi(argv[1]);

  print_bool_arg(opt, "opt", clog);
  clog << '\n';

  string app_name = "hamming_dist";
  ifstream is("../" + app_name + "_io_example.txt");
  if (!is)
    throw invalid_argument("failed to open io example file");

  EncryptionParameters params(scheme_type::bfv);
  size_t n = 8192;
  params.set_poly_modulus_degree(n);
  params.set_plain_modulus(PlainModulus::Batching(n, 20));
  ClearArgsInfo clear_inputs, clear_outputs;
  size_t func_slot_count;
  parse_inputs_outputs_file(is, params.plain_modulus().value(), clear_inputs, clear_outputs, func_slot_count);
  params.set_coeff_modulus(CoeffModulus::Create(n, {60, 60, 60}));
  SEALContext context(params, true, sec_level_type::tc128);
  BatchEncoder batch_encoder(context);
  KeyGenerator keygen(context);
  const SecretKey &secret_key = keygen.secret_key();
  PublicKey public_key;
  keygen.create_public_key(public_key);
  RelinKeys relin_keys;
  keygen.create_relin_keys(relin_keys);
  GaloisKeys galois_keys;

  keygen.create_galois_keys(get_rotation_steps_hamming_dist(), galois_keys);

  Encryptor encryptor(context, public_key);
  Evaluator evaluator(context);
  Decryptor decryptor(context, secret_key);

  EncryptedArgs encrypted_inputs;
  EncodedArgs encoded_inputs;
  prepare_he_inputs(batch_encoder, encryptor, clear_inputs, encrypted_inputs, encoded_inputs);
  EncryptedArgs encrypted_outputs;
  EncodedArgs encoded_outputs;

  chrono::high_resolution_clock::time_point t;
  chrono::duration<double, milli> elapsed;
  t = chrono::high_resolution_clock::now();

  hamming_dist(
    encrypted_inputs, encoded_inputs, encrypted_outputs, encoded_outputs, batch_encoder, encryptor, evaluator,
    relin_keys, galois_keys);

  elapsed = chrono::high_resolution_clock::now() - t;

  ClearArgsInfo obtained_clear_outputs;
  get_clear_outputs(
    batch_encoder, decryptor, encrypted_outputs, encoded_outputs, func_slot_count, obtained_clear_outputs);
  print_encrypted_outputs_info(context, decryptor, encrypted_outputs, clog);
  if (clear_outputs != obtained_clear_outputs)
    throw logic_error("clear_outputs != obtained_clear_outputs");

  cout << elapsed.count() << " ms\n";
}
