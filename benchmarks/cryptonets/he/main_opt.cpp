#include <chrono>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iostream>
#include "gen_he_cryptonets_opt.hpp"
#include "utils.hpp"

using namespace std;
using namespace seal;

int main(int argc, char **argv)
{
  chrono::high_resolution_clock::time_point t;
  chrono::duration<double, milli> elapsed;
  // calculate scaling factors
  const size_t in_precis = 6;
  const size_t w1_precis = in_precis;
  const size_t w4_precis = in_precis;
  const size_t w8_precis = in_precis;
  const size_t b1_precis = in_precis + w1_precis;
  const size_t b4_precis = 2 * b1_precis + w4_precis + 2;
  const size_t b8_precis = 2 * b4_precis + w8_precis + 4;
  const Number in_scaler = 1 << in_precis;
  const Number w1_scaler = 1 << w1_precis;
  const Number w4_scaler = 1 << w4_precis;
  const Number w8_scaler = 1 << w8_precis;
  const Number b1_scaler = shift(static_cast<Number>(1), b1_precis);
  const Number b4_scaler = shift(static_cast<Number>(1), b4_precis);
  const Number b8_scaler = shift(static_cast<Number>(1), b8_precis);
  clog << "precision (bits):\n";
  clog << "inputs: " << in_precis << '\n';
  clog << "weights: " << w1_precis << " " << w4_precis << " " << w8_precis << '\n';
  clog << "biases: " << b1_precis << " " << b4_precis << " " << b8_precis << '\n';
  // open data files
  ifstream x_is("data/x_test.tmp");
  if (!x_is)
    throw invalid_argument("failed to open x_test file");

  ifstream y_is("data/y_test.tmp");
  if (!y_is)
    throw invalid_argument("failed to open y_test file");

  ifstream w1_is("data/W1.tmp");
  if (!w1_is)
    throw invalid_argument("failed to open W1 file");

  ifstream w4_is("data/W4.tmp");
  if (!w4_is)
    throw invalid_argument("failed to open W4 file");

  ifstream w8_is("data/W8.tmp");
  if (!w8_is)
    throw invalid_argument("failed to open W8 file");

  ifstream b1_is("data/b1.tmp");
  if (!b1_is)
    throw invalid_argument("failed to open b1 file");

  ifstream b4_is("data/b4.tmp");
  if (!b4_is)
    throw invalid_argument("failed to open b4 file");

  ifstream b8_is("data/b8.tmp");
  if (!b8_is)
    throw invalid_argument("failed to open b8 file");

  // load clear data
  clog << "load clear data .. " << flush;
  t = chrono::high_resolution_clock::now();
  vector<vector<vector<vector<double>>>> x_clear;
  {
    auto x = load(x_is, ',');
    x_clear = reshape_4d(x, {x.size(), 28, 28, 1});
  }
  auto y_clear = load<size_t>(y_is);
  vector<vector<vector<vector<double>>>> w1_clear;
  {
    auto w1 = load(w1_is, ',');
    w1_clear = reshape_4d(w1, {w1.size(), 5, 1, 5});
  }
  vector<vector<vector<vector<double>>>> w4_clear;
  {
    auto w4 = load(w4_is, ',');
    w4_clear = reshape_4d(w4, {w4.size(), 5, 5, 10});
  }
  auto w8_clear = load(w8_is, ',');
  auto b1_clear = load<double>(b1_is);
  auto b4_clear = load<double>(b4_is);
  auto b8_clear = load<double>(b8_is);
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
  // scale data
  clog << "scale .. " << flush;
  t = chrono::high_resolution_clock::now();
  auto x_clear_scaled = scale(x_clear, in_scaler);
  x_clear_scaled = reshape_order(x_clear_scaled, {1, 2, 3, 0});
  auto w1_clear_scaled = scale(w1_clear, w1_scaler);
  auto w4_clear_scaled = scale(w4_clear, w4_scaler);
  auto w8_clear_scaled = scale(w8_clear, w8_scaler);
  auto b1_clear_scaled = scale(b1_clear, b1_scaler);
  auto b4_clear_scaled = scale(b4_clear, b4_scaler);
  auto b8_clear_scaled = scale(b8_clear, b8_scaler);
  elapsed = chrono::high_resolution_clock::now() - t;
  clog << "ok (" << elapsed.count() << " ms)\n";
  // prepare data (encode with crt/encrypt) and run
  const vector<uint64_t> coprimes{65537, 114689, 147457, 163841, 557057};
  Number modulus = 1;
  for (auto prime : coprimes)
    modulus *= prime;
  vector<unique_ptr<BatchEncoder>> encoders;
  encoders.reserve(coprimes.size());
  vector<unique_ptr<Decryptor>> decryptors;
  decryptors.reserve(coprimes.size());
  vector<EncryptedArgs> primes_encrypted_outputs;
  primes_encrypted_outputs.reserve(coprimes.size());
  for (size_t i = 0; i < coprimes.size(); ++i)
  {
    // create context and required seal objects
    clog << "coprime " << coprimes[i] << '\n';
    EncryptionParameters params(scheme_type::bfv);
    params.set_poly_modulus_degree(n);
    params.set_plain_modulus(coprimes[i]);
    params.set_coeff_modulus(CoeffModulus::BFVDefault(n));
    SEALContext context(params, false, sec_level_type::tc128);
    encoders.push_back(make_unique<BatchEncoder>(context));
    KeyGenerator keygen(context);
    const SecretKey &secret_key = keygen.secret_key();
    PublicKey public_key;
    keygen.create_public_key(public_key);
    RelinKeys relin_keys;
    keygen.create_relin_keys(relin_keys);
    GaloisKeys galois_keys;
    Encryptor encryptor(context, public_key);
    Evaluator evaluator(context);
    decryptors.push_back(make_unique<Decryptor>(context, secret_key));
    // encode/encrypt inputs
    clog << "encode/encrypt .. " << flush;
    t = chrono::high_resolution_clock::now();
    EncryptedArgs encrypted_inputs;
    EncodedArgs encoded_inputs;
    prepare_he_inputs(
      modulus, coprimes[i], *encoders[i], encryptor, x_clear_scaled, w1_clear_scaled, w4_clear_scaled, w8_clear_scaled,
      b1_clear_scaled, b4_clear_scaled, b8_clear_scaled, encrypted_inputs, encoded_inputs);

    elapsed = chrono::high_resolution_clock::now() - t;
    clog << "ok (" << elapsed.count() << " ms)\n";
    // run he evaluation
    clog << "evaluate .. " << flush;
    t = chrono::high_resolution_clock::now();
    EncodedArgs encoded_outputs;
    primes_encrypted_outputs.push_back(EncryptedArgs{});
    cryptonets_opt(
      encrypted_inputs, encoded_inputs, primes_encrypted_outputs[i], encoded_outputs, *encoders[i], encryptor,
      evaluator, relin_keys, galois_keys);
    elapsed = chrono::high_resolution_clock::now() - t;
    clog << "ok (" << elapsed.count() << " ms)\n";
    print_encrypted_outputs_info(context, *decryptors[i], primes_encrypted_outputs[i], clog);
  }
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
  clog << "The accuracy on test data for MNIST: " << acc << '\n';
}
