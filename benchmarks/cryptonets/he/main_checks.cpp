#include <chrono>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <ostream>
#include <unordered_map>
#include "gen/gen_he_cryptonets_114689_noopt.hpp"
#include "gen/gen_he_cryptonets_114689_opt.hpp"
#include "gen/gen_he_cryptonets_147457_noopt.hpp"
#include "gen/gen_he_cryptonets_147457_opt.hpp"
#include "gen/gen_he_cryptonets_163841_noopt.hpp"
#include "gen/gen_he_cryptonets_163841_opt.hpp"
#include "gen/gen_he_cryptonets_557057_noopt.hpp"
#include "gen/gen_he_cryptonets_557057_opt.hpp"
#include "gen/gen_he_cryptonets_65537_noopt.hpp"
#include "gen/gen_he_cryptonets_65537_opt.hpp"
#include "utils_checks.hpp"

using namespace std;
using namespace seal;

using EvalFunc = void (*)(
  const unordered_map<string, Ciphertext> &, const unordered_map<string, Plaintext> &,
  unordered_map<string, Ciphertext> &, unordered_map<string, Plaintext> &, const BatchEncoder &, const Encryptor &,
  const Evaluator &, const RelinKeys &, const GaloisKeys &);

using RotationKeyFunc = vector<int> (*)();

void print_bool_arg(bool arg, const string &name, ostream &os)
{
  os << (arg ? name : "no" + name);
}

int main(int argc, char **argv)
{
  uint64_t modulus = 65537;
  if (argc > 1)
    modulus = stoull(argv[1]);

  bool opt = true;
  if (argc > 2)
    opt = stoi(argv[2]);

  clog << modulus;
  clog << " ";
  print_bool_arg(opt, "opt", clog);
  clog << '\n';

  string app_name = "cryptonets_" + to_string(modulus);
  ifstream is("../io_examples/" + app_name + "_io_example.txt");
  if (!is)
    throw invalid_argument("failed to open io example file");

  const unordered_map<uint64_t, EvalFunc> opt_eval_funcs = {
    {65537, &cryptonets_65537_opt},
    {114689, &cryptonets_114689_opt},
    {147457, &cryptonets_147457_opt},
    {163841, &cryptonets_163841_opt},
    {557057, &cryptonets_557057_opt}};
  const unordered_map<uint64_t, EvalFunc> noopt_eval_funcs = {
    {65537, &cryptonets_65537_noopt},
    {114689, &cryptonets_114689_noopt},
    {147457, &cryptonets_147457_noopt},
    {163841, &cryptonets_163841_noopt},
    {557057, &cryptonets_557057_noopt}};
  const unordered_map<uint64_t, RotationKeyFunc> opt_rotation_key_funcs = {
    {65537, &get_rotation_steps_cryptonets_65537_opt},
    {114689, &get_rotation_steps_cryptonets_114689_opt},
    {147457, &get_rotation_steps_cryptonets_147457_opt},
    {163841, &get_rotation_steps_cryptonets_163841_opt},
    {557057, &get_rotation_steps_cryptonets_557057_opt}};
  const unordered_map<uint64_t, RotationKeyFunc> noopt_rotation_key_funcs = {
    {65537, &get_rotation_steps_cryptonets_65537_noopt},
    {114689, &get_rotation_steps_cryptonets_114689_noopt},
    {147457, &get_rotation_steps_cryptonets_147457_noopt},
    {163841, &get_rotation_steps_cryptonets_163841_noopt},
    {557057, &get_rotation_steps_cryptonets_557057_noopt}};

  EncryptionParameters params(scheme_type::bfv);
  size_t n = 8192;
  params.set_poly_modulus_degree(n);
  params.set_plain_modulus(modulus);
  ClearArgsInfo clear_inputs, clear_outputs;
  size_t func_slot_count;
  parse_inputs_outputs_file(is, params.plain_modulus().value(), clear_inputs, clear_outputs, func_slot_count);
  params.set_coeff_modulus(CoeffModulus::BFVDefault(n));
  SEALContext context(params, true, sec_level_type::tc128);
  BatchEncoder batch_encoder(context);
  KeyGenerator keygen(context);
  const SecretKey &secret_key = keygen.secret_key();
  PublicKey public_key;
  keygen.create_public_key(public_key);
  RelinKeys relin_keys;
  keygen.create_relin_keys(relin_keys);
  GaloisKeys galois_keys;
  if (opt)
    keygen.create_galois_keys(opt_rotation_key_funcs.at(modulus)(), galois_keys);
  else
    keygen.create_galois_keys(noopt_rotation_key_funcs.at(modulus)(), galois_keys);
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
  if (opt)
    opt_eval_funcs.at(modulus)(
      encrypted_inputs, encoded_inputs, encrypted_outputs, encoded_outputs, batch_encoder, encryptor, evaluator,
      relin_keys, galois_keys);
  else
    noopt_eval_funcs.at(modulus)(
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
