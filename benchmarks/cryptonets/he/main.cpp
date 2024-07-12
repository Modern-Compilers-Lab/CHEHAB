#include <chrono>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <ostream>
#include "_gen_he_cryptonets_114689.hpp"
#include "_gen_he_cryptonets_147457.hpp"
#include "_gen_he_cryptonets_163841.hpp"
#include "_gen_he_cryptonets_557057.hpp"
#include "_gen_he_cryptonets_65537.hpp"
#include "utils.hpp"

using namespace std;
using namespace seal;

using EvalFunc = void (*)(
  const unordered_map<string, Ciphertext> &, const unordered_map<string, Plaintext> &,
  unordered_map<string, Ciphertext> &, unordered_map<string, Plaintext> &, const BatchEncoder &, const Encryptor &,
  const Evaluator &, const RelinKeys &, const GaloisKeys &);

using RotationKeyFunc = vector<int> (*)();

void print_bool_arg(bool arg, const string &name, ostream &os)
{
  os << (arg ? name : "no_" + name);
}

int main(int argc, char **argv)
{
  bool opt = true;
  if (argc > 1)
    opt = stoi(argv[1]);

  bool eval = true;
  if (argc > 2)
    eval = stoi(argv[2]);

  print_bool_arg(opt, "opt", clog);
  clog << " ";
  print_bool_arg(eval, "eval", clog);
  clog << '\n';

  chrono::high_resolution_clock::time_point t;
  chrono::duration<double, milli> elapsed;
  // open data files
  string data_loc = "data/";
  ifstream x_is(data_loc + "x_test.tmp");
  if (!x_is)
    throw invalid_argument("failed to open x_test file");

  ifstream y_is(data_loc + "y_test.txt");
  if (!y_is)
    throw invalid_argument("failed to open y_test file");

  // load
  clog << "load clear data .. " << flush;
  t = chrono::high_resolution_clock::now();
  vector<vector<vector<vector<double>>>> x_clear;
  {
    auto x = load(x_is, ',');
    x_clear = reshape_4d(x, {x.size(), 28, 28, 1});
  }
  auto y_clear = load<size_t>(y_is);
  elapsed = chrono::high_resolution_clock::now() - t;
  clog << "ok (" << elapsed.count() << " ms)\n";
  // take subset
  const size_t n = 8192;
  x_clear = subset(x_clear, 0, n);
  y_clear = subset(y_clear, 0, n);
  clog << "take " << n << " first inputs (batch size n)\n";
  print_vec(shape(x_clear), clog);
  clog << '\n';
  print_vec(shape(y_clear), clog);
  clog << '\n';
  // compute scaling factors
  const int in_precis = 6;
  const int w1_precis = in_precis;
  const int w4_precis = in_precis;
  const int w8_precis = in_precis;
  const int b1_precis = in_precis + w1_precis;
  const int b4_precis = 2 * b1_precis + w4_precis + 2;
  const int b8_precis = 2 * b4_precis + w8_precis + 4;
  const Number in_scaler = shift(static_cast<Number>(1), in_precis);
  clog << "precision (bits):\n";
  clog << "inputs: " << in_precis << '\n';
  clog << "weights: " << w1_precis << " " << w4_precis << " " << w8_precis << '\n';
  clog << "biases: " << b1_precis << " " << b4_precis << " " << b8_precis << '\n';
  // scale
  clog << "scale" << '\n';
  auto x_clear_scaled = scale(x_clear, in_scaler);
  // reshape_order
  x_clear_scaled = reshape_order(x_clear_scaled, {1, 2, 3, 0});
  // prepare data (encode with crt/encrypt) then evaluate
  const vector<uint64_t> coprimes{65537, 114689, 147457, 163841, 557057};
  const vector<EvalFunc> eval_funcs = {
    &cryptonets_65537, &cryptonets_114689, &cryptonets_147457, &cryptonets_163841, &cryptonets_557057};

  const vector<RotationKeyFunc> rotation_key_funcs = {
    &get_rotation_steps_cryptonets_65537, &get_rotation_steps_cryptonets_114689, &get_rotation_steps_cryptonets_147457,
    &get_rotation_steps_cryptonets_163841, &get_rotation_steps_cryptonets_557057};

  Number modulus = 1;
  for (auto prime : coprimes)
    modulus *= prime;
  vector<unique_ptr<BatchEncoder>> encoders;
  encoders.reserve(coprimes.size());
  vector<unique_ptr<Decryptor>> decryptors;
  decryptors.reserve(coprimes.size());
  vector<EncryptedArgs> primes_encrypted_outputs;
  primes_encrypted_outputs.reserve(coprimes.size());
  chrono::duration<double, milli> eval_time(0);
  for (size_t i = 0; i < coprimes.size(); ++i)
  {
    clog << "coprime " << coprimes[i] << '\n';
    if (eval)
    {
      // create context, required seal objects and keys
      clog << "create context, api objects and keys .. " << flush;
      t = chrono::high_resolution_clock::now();
      EncryptionParameters params(scheme_type::bfv);
      params.set_poly_modulus_degree(n);
      params.set_plain_modulus(coprimes[i]);
      params.set_coeff_modulus(CoeffModulus::BFVDefault(n));
      SEALContext context(params, true, sec_level_type::tc128);
      encoders.push_back(make_unique<BatchEncoder>(context));
      KeyGenerator keygen(context);
      const SecretKey &secret_key = keygen.secret_key();
      PublicKey public_key;
      keygen.create_public_key(public_key);
      RelinKeys relin_keys;
      keygen.create_relin_keys(relin_keys);
      GaloisKeys galois_keys;

      keygen.create_galois_keys(rotation_key_funcs[i](), galois_keys);

      Encryptor encryptor(context, public_key);
      Evaluator evaluator(context);
      decryptors.push_back(make_unique<Decryptor>(context, secret_key));
      clog << "ok (" << elapsed.count() << " ms)\n";
      // encode/encrypt inputs
      clog << "encode/encrypt .. " << flush;
      t = chrono::high_resolution_clock::now();
      EncryptedArgs encrypted_inputs;
      prepare_he_inputs(modulus, coprimes[i], *encoders[i], encryptor, x_clear_scaled, encrypted_inputs);
      EncodedArgs encoded_inputs;
      elapsed = chrono::high_resolution_clock::now() - t;
      clog << "ok (" << elapsed.count() << " ms)\n";
      // run he evaluation
      clog << "evaluate .. " << flush;
      t = chrono::high_resolution_clock::now();
      EncodedArgs encoded_outputs;
      primes_encrypted_outputs.push_back(EncryptedArgs{});

      eval_funcs[i](
        encrypted_inputs, encoded_inputs, primes_encrypted_outputs[i], encoded_outputs, *encoders[i], encryptor,
        evaluator, relin_keys, galois_keys);

      elapsed = chrono::high_resolution_clock::now() - t;
      eval_time += elapsed;
      clog << "ok (" << elapsed.count() << " ms)\n";
      print_encrypted_outputs_info(context, *decryptors[i], primes_encrypted_outputs[i], clog);
    }
    else
    {
      clog << "export reduced weights and biases\n";
      export_reduced_weights_biases(
        modulus, coprimes[i], w1_precis, w4_precis, w8_precis, b1_precis, b4_precis, b8_precis);
    }
  }
  if (eval)
  {
    // decrypt
    clog << "decrypt .. " << flush;
    t = chrono::high_resolution_clock::now();
    auto raw_y = get_clear_outputs(coprimes, modulus, encoders, decryptors, primes_encrypted_outputs);
    elapsed = chrono::high_resolution_clock::now() - t;
    clog << "ok (" << elapsed.count() << " ms)\n";
    raw_y = reshape_order(raw_y, {1, 0});
    // plain_plain_ops
    auto obtained_y_clear = argmax(raw_y);
    auto acc = static_cast<double>(count_equal(obtained_y_clear, y_clear)) / obtained_y_clear.size();
    clog << "the accuracy on test data for MNIST: " << acc << '\n';
    cout << eval_time.count() << " ms\n";
  }
}
