#include "seal/seal.h"
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>
#include <tuple>
#include <vector>

using namespace std;
using namespace seal;

tuple<int, vector<int>> get_params_from_xdepth_bgv_128(int xdepth)
{
  static const map<int, tuple<int, vector<int>>> params_from_xdepth_bgv_128{

    /*
    xdepth:10
    Polynomial modulus: 16384
    Modulus count: 8
    Total bit count: 438
    */
    {10, {16384, {55, 55, 54, 54, 55, 55, 55, 55}}}

  };

  return params_from_xdepth_bgv_128.at(xdepth);
}

tuple<int64_t, int64_t> bfv_test(SEALContext context, int xdepth)
{
  chrono::high_resolution_clock::time_point time_start, time_end;
  chrono::microseconds time_mul(0);
  chrono::microseconds time_relin(0);
  chrono::microseconds time_mod_switch(0);

  BatchEncoder batch_encoder(context);
  KeyGenerator keygen(context);
  SecretKey secret_key = keygen.secret_key();
  PublicKey public_key;
  keygen.create_public_key(public_key);
  RelinKeys relin_keys;
  keygen.create_relin_keys(relin_keys);
  Encryptor encryptor(context, public_key);
  Evaluator evaluator(context);
  Decryptor decryptor(context, secret_key);

  size_t slot_count = batch_encoder.slot_count();
  vector<uint64_t> pod_vector;
  random_device rd;
  for (size_t i = 0; i < slot_count; i++)
    pod_vector.push_back(context.first_context_data()->parms().plain_modulus().reduce(rd()));
  Plaintext plain(context.first_context_data()->parms().poly_modulus_degree(), 0);
  batch_encoder.encode(pod_vector, plain);
  Ciphertext encrypted(context);
  encryptor.encrypt(plain, encrypted);

  for (int i = 0; i < xdepth; i++)
  {
    time_start = chrono::high_resolution_clock::now();
    evaluator.multiply_inplace(encrypted, encrypted);
    time_end = chrono::high_resolution_clock::now();
    time_mul += chrono::duration_cast<chrono::microseconds>(time_end - time_start);

    time_start = chrono::high_resolution_clock::now();
    evaluator.relinearize_inplace(encrypted, relin_keys);
    time_end = chrono::high_resolution_clock::now();
    time_relin += chrono::duration_cast<chrono::microseconds>(time_end - time_start);
    if (i % 2)
    {
      time_start = chrono::high_resolution_clock::now();
      evaluator.mod_switch_to_next_inplace(encrypted);
      time_end = chrono::high_resolution_clock::now();
      time_mod_switch += chrono::duration_cast<chrono::microseconds>(time_end - time_start);
    }
  }
  if (decryptor.invariant_noise_budget(encrypted) == 0)
    throw logic_error("insufficient noise budget");

  int64_t avg_mul = time_mul.count() / xdepth;
  int64_t avg_relin = time_relin.count() / xdepth;
  int64_t avg_mod_switch = time_mod_switch.count() / 5;
  cout << "Average mul: " << avg_mul << " microseconds" << endl;
  cout << "Average relin: " << avg_relin << " microseconds" << endl;
  cout << "Average modswitch: " << avg_mod_switch << " microseconds" << endl;
  return {avg_mul, avg_relin};
}

int main()
{
  EncryptionParameters parms(scheme_type::bfv);
  size_t poly_modulus_degree = 16384;
  parms.set_poly_modulus_degree(poly_modulus_degree);
  parms.set_coeff_modulus(CoeffModulus::Create(poly_modulus_degree, {55, 54, 54, 54, 55, 55, 55, 55}));
  Modulus plain_modulus = PlainModulus::Batching(poly_modulus_degree, 20);
  parms.set_plain_modulus(plain_modulus);
  SEALContext context(parms);
  bfv_test(context, 10);
  // cout << "Average mul: " << avg_mul << " microseconds" << endl;
  // cout << "Average relin: " << avg_relin << " microseconds" << endl;
  return 0;
}
