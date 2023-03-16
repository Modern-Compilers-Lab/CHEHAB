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
  int xdepth, bool use_least_levels = false, sec_level_type sec_level = sec_level_type::tc128,
  size_t initial_poly_md = 2048, int initial_plain_m_size = 17)
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
    if (use_least_levels)
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
    else
    {
      int nb_primes = xdepth + 1;
      int avg_prime_size = coeff_m_data_level_bc / nb_primes;
      coeff_m_bit_sizes.assign(nb_primes, avg_prime_size);
      // Remove exceeding bits
      for (int i = 0; i < coeff_m_data_level_bc % nb_primes; ++i)
        ++coeff_m_bit_sizes.end()[-1 - i];
      // Add special modulus for ks
      coeff_m_bit_sizes.push_back(coeff_m_bit_sizes.back());
    }
  }

  // If for the supposed n and the given security level q size cannot reach the needed value
  if (
    CoeffModulus::MaxBitCount(poly_modulus_degree, sec_level) <
    coeff_m_data_level_bc + ((coeff_m_bit_sizes.size() > 1) ? coeff_m_bit_sizes.back() : 0))
  {
    if (poly_modulus_degree == max_poly_md)
      throw invalid_argument("depth too large, corresponding parameters not included in fhe security standard");
    //  Repeat with initial n=2*current_n
    return select_params_bfv(xdepth, use_least_levels, sec_level, poly_modulus_degree * 2, plain_m_size);
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
    return select_params_bfv(xdepth, use_least_levels, sec_level, poly_modulus_degree, plain_m_size + 1);
  }

  coeff_modulus = CoeffModulus::Create(poly_modulus_degree, coeff_m_bit_sizes);
  // Create context from the selected parameters
  EncryptionParameters params(scheme_type::bfv);
  params.set_poly_modulus_degree(poly_modulus_degree);
  params.set_coeff_modulus(coeff_modulus);
  params.set_plain_modulus(plain_modulus);
  return params;
}

void mul_benchmark(SEALContext context, int xdepth, int repeat = 10)
{
  if (context.first_context_data()->parms().coeff_modulus().size() != xdepth + 1)
    throw invalid_argument("Coeff_modulus levels different than xdepth");

  chrono::high_resolution_clock::time_point time_start, time_end;
  vector<chrono::microseconds> time_mul_levels(xdepth);
  vector<chrono::microseconds> time_relin_levels(xdepth);
  vector<chrono::microseconds> time_mod_switch_levels(xdepth);

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

  Ciphertext tmp(context);

  for (int i = 0; i < xdepth; i++)
  {
    // Multiplication time per level
    for (int j = 0; j < repeat - 1; ++j)
    {
      time_start = chrono::high_resolution_clock::now();
      evaluator.multiply(encrypted, encrypted, tmp);
      time_end = chrono::high_resolution_clock::now();
      time_mul_levels[i] += chrono::duration_cast<chrono::microseconds>(time_end - time_start);
    }
    time_start = chrono::high_resolution_clock::now();
    evaluator.multiply_inplace(encrypted, encrypted);
    time_end = chrono::high_resolution_clock::now();
    time_mul_levels[i] += chrono::duration_cast<chrono::microseconds>(time_end - time_start);
    // Relinearization time per level
    for (int j = 0; j < repeat - 1; ++j)
    {
      time_start = chrono::high_resolution_clock::now();
      evaluator.relinearize(encrypted, relin_keys, tmp);
      time_end = chrono::high_resolution_clock::now();
      time_relin_levels[i] += chrono::duration_cast<chrono::microseconds>(time_end - time_start);
    }
    time_start = chrono::high_resolution_clock::now();
    evaluator.relinearize_inplace(encrypted, relin_keys);
    time_end = chrono::high_resolution_clock::now();
    time_relin_levels[i] += chrono::duration_cast<chrono::microseconds>(time_end - time_start);
    // Modulus switching time per level
    for (int j = 0; j < repeat - 1; ++j)
    {
      time_start = chrono::high_resolution_clock::now();
      evaluator.mod_switch_to_next(encrypted, tmp);
      time_end = chrono::high_resolution_clock::now();
      time_mod_switch_levels[i] += chrono::duration_cast<chrono::microseconds>(time_end - time_start);
    }
    time_start = chrono::high_resolution_clock::now();
    evaluator.mod_switch_to_next_inplace(encrypted);
    time_end = chrono::high_resolution_clock::now();
    time_mod_switch_levels[i] += chrono::duration_cast<chrono::microseconds>(time_end - time_start);
  }
  if (decryptor.invariant_noise_budget(encrypted) == 0)
    throw logic_error("insufficient noise budget");

  vector<int64_t> avg_mul_levels(xdepth);
  vector<int64_t> avg_relin_levels(xdepth);
  vector<int64_t> avg_mod_switch_levels(xdepth);
  for (int i = 0; i < xdepth; ++i)
  {
    avg_mul_levels[i] = time_mul_levels[i].count() / repeat;
    avg_relin_levels[i] = time_relin_levels[i].count() / repeat;
    avg_mod_switch_levels[i] = time_mod_switch_levels[i].count() / repeat;
  }

  // Print resutls
  cout << "Average mul: ";
  for (int i = 0; i < xdepth; ++i)
    cout << avg_mul_levels[i] << " ";
  cout << endl;
  cout << "Average relin: ";
  for (int i = 0; i < xdepth; ++i)
    cout << avg_relin_levels[i] << " ";
  cout << endl;
  cout << "Average modswitch: ";
  for (int i = 0; i < xdepth; ++i)
    cout << avg_mod_switch_levels[i] << " ";
  cout << endl;
}

int main(int argc, char **argv)
{
  // FHE security standard allows up to 23
  int xdepth = 20;
  bool use_least_levels = false;
  if (argc > 1)
  {
    xdepth = atoi(argv[1]);
    if (argc > 2)
      use_least_levels = atoi(argv[2]);
  }
  EncryptionParameters params(scheme_type::bfv);
  size_t poly_modulus_degree = 32768;
  params.set_poly_modulus_degree(poly_modulus_degree);
  // xdepth=20
  int safety_margin = 3;
  int min_data_level_coeff_m_bc = 648 + safety_margin;
  int nb_primes = xdepth + 1;
  vector<int> coeff_m_bit_counts(nb_primes, min_data_level_coeff_m_bc / nb_primes);
  for (int i = 0; i < min_data_level_coeff_m_bc % nb_primes; ++i)
    ++coeff_m_bit_counts.end()[-1 - i];
  // Special prime
  coeff_m_bit_counts.push_back(coeff_m_bit_counts.back());
  params.set_coeff_modulus(CoeffModulus::Create(poly_modulus_degree, coeff_m_bit_counts));
  Modulus plain_modulus = 65537;
  params.set_plain_modulus(plain_modulus);
  // cout << "Average mul: " << avg_mul << " microseconds" << endl;
  // cout << "Average relin: " << avg_relin << " microseconds" << endl;*/
  // EncryptionParameters params = select_params_bfv(xdepth, use_least_levels);
  SEALContext context(params);
  print_parameters(context);
  mul_benchmark(context, xdepth);
  return 0;
}
