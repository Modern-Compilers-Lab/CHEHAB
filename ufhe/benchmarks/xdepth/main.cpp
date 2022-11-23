#include "seal/seal.h"
#include "utils.hpp"
#include <chrono>
#include <cstddef>
#include <iostream>
#include <random>
#include <stdexcept>
#include <vector>

using namespace std;
using namespace seal;

EncryptionParameters select_params_bfv(
  int xdepth, sec_level_type sec_level = sec_level_type::tc128, size_t initial_poly_md = 2048,
  int initial_plain_m_size = 17)
{
  if (initial_plain_m_size < 17)
    throw invalid_argument("plain modulus size must be at least 17 bits");

  size_t max_poly_md = 32768;
  int max_fresh_noise = 10;
  int safety_margin = 1;

  size_t poly_modulus_degree = initial_poly_md;
  vector<Modulus> coeff_modulus;
  Modulus plain_modulus;
  int plain_m_size = initial_plain_m_size;

  // Set coeff modulus primes sizes
  int worst_case_mul_ng = util::get_significant_bit_count(poly_modulus_degree) * 2 + plain_m_size + 1;
  int best_case_mul_ng = plain_m_size + 1;
  int avg_mul_ng = (worst_case_mul_ng + best_case_mul_ng) / 2 + (worst_case_mul_ng + best_case_mul_ng) % 2;
  int coeff_m_data_level_bc = plain_m_size + max_fresh_noise + xdepth * avg_mul_ng + safety_margin;

  vector<int> coeff_m_bit_sizes;
  // No special prime
  if (xdepth <= 1 && coeff_m_data_level_bc <= 60)
    coeff_m_bit_sizes.assign(1, coeff_m_data_level_bc);
  else
  {
    int lowest_nb_levels = coeff_m_data_level_bc / 60;
    if (coeff_m_data_level_bc % 60)
      ++lowest_nb_levels;
    coeff_m_bit_sizes.assign(lowest_nb_levels, 60);
    // Remove exceeding bits
    for (int i = 0; i < 60 - (coeff_m_data_level_bc % 60); ++i)
      --coeff_m_bit_sizes[i % coeff_m_bit_sizes.size()];
    // Add special modulus for ks
    coeff_m_bit_sizes.push_back(coeff_m_bit_sizes.back());
  }

  // If for the supposed n and the given security level q size cannot reach the needed value
  if (
    CoeffModulus::MaxBitCount(poly_modulus_degree, sec_level) <
    coeff_m_data_level_bc + ((coeff_m_bit_sizes.size() > 1) ? coeff_m_bit_sizes.back() : 0))
  {
    if (poly_modulus_degree == max_poly_md)
      throw invalid_argument("depth too large, corresponding parameters not included in fhe security standard");
    //  Repeat with initial n=2*current_n
    return select_params_bfv(xdepth, sec_level, poly_modulus_degree * 2, plain_m_size);
  }

  try
  {
    plain_modulus = PlainModulus::Batching(poly_modulus_degree, plain_m_size);
  }
  // suitable prime could not be found
  catch (logic_error &e)
  {
    if (plain_m_size == 60)
      throw invalid_argument("depth too large, corresponding parameters not included in fhe security standard");
    //  Repeat with incremented initial plain modulus size
    return select_params_bfv(xdepth, sec_level, poly_modulus_degree, plain_m_size + 1);
  }

  coeff_modulus = CoeffModulus::Create(poly_modulus_degree, coeff_m_bit_sizes);
  // Create context from the selected parameters
  EncryptionParameters params(scheme_type::bfv);
  params.set_poly_modulus_degree(poly_modulus_degree);
  params.set_coeff_modulus(coeff_modulus);
  params.set_plain_modulus(plain_modulus);
  return params;
}

void bfv_mul_bencmark(SEALContext context, int xdepth)
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
}

int main(int argc, char **argv)
{
  int xdepth = 23;
  if (argc > 1)
    xdepth = atoi(argv[1]);
  /*EncryptionParameters parms(scheme_type::bfv);
  size_t poly_modulus_degree = 16384;
  parms.set_poly_modulus_degree(poly_modulus_degree);
  parms.set_coeff_modulus(CoeffModulus::Create(poly_modulus_degree, {55, 54, 54, 54, 55, 55, 55, 55}));
  Modulus plain_modulus = PlainModulus::Batching(poly_modulus_degree, 20);
  parms.set_plain_modulus(plain_modulus);
  SEALContext context(parms);
  bfv_test(context, 10);
  // cout << "Average mul: " << avg_mul << " microseconds" << endl;
  // cout << "Average relin: " << avg_relin << " microseconds" << endl;*/
  EncryptionParameters params = select_params_bfv(xdepth);
  SEALContext context(params);
  print_parameters(context);
  return 0;
}
