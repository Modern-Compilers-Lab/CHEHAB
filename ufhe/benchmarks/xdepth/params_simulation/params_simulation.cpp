
#include "params_simulation.hpp"
#include <cmath>
#include <iostream>
#include <random>
#include <stdexcept>

#define MIN_PARAMS_PRIMES_ERROR "minimal parameters primes could not be created"
#define MIN_PARAMS_NOT_VALID "Minimal parameters not valid"
#define MAX_MOD_SIZE 60

using namespace std;
using namespace seal;

tuple<SEALContext, vector<int>> bfv_params_simulation(
  int init_plain_mod_size, int xdepth, int safety_margin, sec_level_type sec_level)
{
  cout << "initial plain modulus size: " << init_plain_mod_size << endl;
  cout << "xdepth: " << xdepth << endl;
  cout << "Safety margin: " << safety_margin << endl;
  cout << endl;

  // Set Initial parameters using using bfv_params_heuristic
  auto result = bfv_params_heuristic(init_plain_mod_size, xdepth, sec_level);
  // Initial coeff_mod_primes_sizes
  EncryptionParameters params = get<0>(result);
  float estimated_mul_ng = get<1>(result);
  vector<int> coeff_mod_primes_sizes(params.coeff_modulus().size());
  int coeff_mod_total_size = 0;
  for (int i = 0; i < coeff_mod_primes_sizes.size(); ++i)
  {
    coeff_mod_primes_sizes[i] = params.coeff_modulus()[i].bit_count();
    coeff_mod_total_size += coeff_mod_primes_sizes[i];
  }
  // Initial poly_mod
  size_t poly_mod = params.poly_modulus_degree();
  // Initial plain_mod
  Modulus plain_mod = params.plain_modulus();

  int test_count = 0;
  do
  {
    // Lambda function to increment parameters
    auto increment_params = [&]() {
      int max_total_size = CoeffModulus::MaxBitCount(poly_mod, sec_level);
      if (coeff_mod_total_size < max_total_size)
      {
        // Increment coeff modulus data level bit count
        int idx = last_small_prime_index(coeff_mod_primes_sizes);
        // Smallest prime size can be incremented
        if (coeff_mod_primes_sizes[idx] < MAX_MOD_SIZE)
        {
          // All primes have the same size
          if (idx == coeff_mod_primes_sizes.size() - 1)
          {
            // Not to increment only special prime size
            if (max_total_size - coeff_mod_total_size > 1)
            {
              ++coeff_mod_primes_sizes[idx];
              ++coeff_mod_primes_sizes[idx - 1];
              coeff_mod_total_size += 2;
            }
            else
            {
              poly_mod <<= 1;
              plain_mod = create_plain_mod(poly_mod, plain_mod.bit_count());
            }
          }
          else
          {
            ++coeff_mod_primes_sizes[idx];
            ++coeff_mod_total_size;
          }
        }
        // All prime sizes have reached 60, add a new prime
        else
        {
          int data_level_size = coeff_mod_total_size - coeff_mod_primes_sizes.back();
          // Remove special prime size
          coeff_mod_primes_sizes.pop_back();
          // Increment the number of data level primes
          int data_level_nb_primes = coeff_mod_primes_sizes.size() + 1;
          // coeff_mod_primes_sizes with only data level primes sizes
          coeff_mod_primes_sizes.assign(data_level_nb_primes, data_level_size / data_level_nb_primes);
          // Add remaining bits and increment
          int remaining_bits = data_level_size % coeff_mod_primes_sizes.size();
          for (int i = 0; i < remaining_bits + 1; ++i)
            ++coeff_mod_primes_sizes.end()[-1 - i];
          // Add special prime
          coeff_mod_primes_sizes.push_back(coeff_mod_primes_sizes.back());
          coeff_mod_total_size = data_level_size + 1 + coeff_mod_primes_sizes.back();
        }
      }
      else
      {
        poly_mod <<= 1;
        plain_mod = create_plain_mod(poly_mod, plain_mod.bit_count());
      }
    };

    // Update EncryptionParameters object with current parameters
    vector<Modulus> coeff_mod;
    try
    {
      coeff_mod = CoeffModulus::Create(poly_mod, coeff_mod_primes_sizes);
    }
    catch (logic_error &e)
    {
      // Not enough suitable primes could be found
      increment_params();
      continue;
    }
    params.set_poly_modulus_degree(poly_mod);
    params.set_coeff_modulus(coeff_mod);
    params.set_plain_modulus(plain_mod);
    // Create SEALContext
    SEALContext context(params, true, sec_level);

    // Test
    vector<int> noise_budgets;
    ++test_count;
    try
    {
      noise_budgets = test_params(context, xdepth);
    }
    // Parameters not sufficient for xdepth
    catch (invalid_argument &e)
    {
      increment_params();
      continue;
    }

    // Valid parameters
    if (test_count == 1)
    {
      cout << "Valid initial parameters" << endl;
      cout << "Estimated mul noise growth: " << estimated_mul_ng << " bits" << endl;
      print_parameters(context);
      cout << endl;
      // Print noise budget progress over the computation
      cout << "Noise budget progress (bits)" << endl;
      print_noise_budget_progress(noise_budgets);
      cout << endl;
    }
    else
      cout << "Valid parameters after " << test_count << " tests" << endl;

    if (noise_budgets.back() < safety_margin)
    {
      for (int i = 0; i < safety_margin - noise_budgets.back(); ++i)
        increment_params();
      // Update EncryptionParameters object with current parameters
      try
      {
        coeff_mod = CoeffModulus::Create(poly_mod, coeff_mod_primes_sizes);
      }
      catch (logic_error &e)
      {
        // Not enough suitable primes could be found
        throw logic_error(MIN_PARAMS_NOT_VALID);
      }
      params.set_poly_modulus_degree(poly_mod);
      params.set_coeff_modulus(coeff_mod);
      params.set_plain_modulus(plain_mod);
      // Create SEALContext
      context = SEALContext(params, true, sec_level);
      // Test just to update noise budget progress
      noise_budgets = test_params(context, xdepth);
    }
    else
    {
      // Reduce coeff modulus bit count
      reduce_coeff_mod_size(coeff_mod_primes_sizes, coeff_mod_total_size, noise_budgets.back() - safety_margin);
      // Reduce poly_mod according to the minimal coeff_mod_total_size and sec_level
      while (CoeffModulus::MaxBitCount(poly_mod >> 1, sec_level) >= coeff_mod_total_size)
        poly_mod >>= 1;
      // If poly_mod is reduced
      if (params.poly_modulus_degree() > poly_mod)
      {
        params.set_poly_modulus_degree(poly_mod);
        // Retry to create plain modulus with size init_plain_mod_size
        plain_mod = create_plain_mod(poly_mod, init_plain_mod_size);
      }
      try
      {
        coeff_mod = CoeffModulus::Create(poly_mod, coeff_mod_primes_sizes);
      }
      catch (logic_error &e)
      {
        // Not enough suitable primes could be found
        throw logic_error(MIN_PARAMS_PRIMES_ERROR);
      }
      params.set_coeff_modulus(coeff_mod);
      params.set_plain_modulus(plain_mod);
      context = SEALContext(params, true, sec_level);
      // Test parameters
      try
      {
        noise_budgets = test_params(context, xdepth);
      }
      // Parameters not sufficient for xdepth
      catch (invalid_argument &e)
      {
        throw logic_error(MIN_PARAMS_NOT_VALID);
      }
    }
    // Print minimal parameters
    cout << "Minimal parameters with margin of " << safety_margin << " bits" << endl;
    print_parameters(context);
    cout << endl;
    // Print noise budget progress over the computation
    cout << "Noise budget progress (bits)" << endl;
    print_noise_budget_progress(noise_budgets);
    return {context, noise_budgets};
  } while (true);
}

tuple<EncryptionParameters, float> bfv_params_heuristic(int init_plain_mod_size, int xdepth, sec_level_type sec_level)
{
  size_t poly_mod = 1024;
  vector<int> coeff_mod_primes_sizes;
  Modulus plain_mod = create_plain_mod(poly_mod, init_plain_mod_size);

  float avg_mul_ng;

  do
  {
    // coeff_mod data level size estimation (without special prime size)
    int max_fresh_noise = 10;
    int poly_mod_size = util::get_significant_bit_count(poly_mod);
    int plain_mod_size = plain_mod.bit_count();
    int worst_case_mul_ng = poly_mod_size * 2 + plain_mod_size + 1;
    int best_case_mul_ng = max(poly_mod_size, plain_mod_size);
    // Calculate the average and round to the next integer
    avg_mul_ng = static_cast<float>(worst_case_mul_ng + best_case_mul_ng) / 2;
    int coeff_mod_data_level_size = plain_mod_size + max_fresh_noise + static_cast<int>(ceilf(xdepth * avg_mul_ng));

    // Set coeff_mod primes sizes (use the lowest number of primes)
    coeff_mod_primes_sizes = split_coeff_mod_lowest_nb_primes(coeff_mod_data_level_size);
    int coeff_mod_total_size = coeff_mod_data_level_size + coeff_mod_primes_sizes.back();

    // If for poly_mod and the given security level, coeff_mod size cannot reach the needed value
    size_t correct_poly_mod = get_poly_mod(coeff_mod_total_size, poly_mod, sec_level);
    if (poly_mod < correct_poly_mod)
    {
      poly_mod = correct_poly_mod;
      // Create plain modulus for batching with new poly_mod
      plain_mod = create_plain_mod(poly_mod, plain_mod_size);
      // Repeat estimation with new poly_mod and plain_mod
      continue;
    }
    // Done
    break;

  } while (true);

  // Create coeff modulus primes using coeff_mod_primes_sizes
  vector<Modulus> coeff_mod = CoeffModulus::Create(poly_mod, coeff_mod_primes_sizes);
  EncryptionParameters params(scheme_type::bfv);
  params.set_poly_modulus_degree(poly_mod);
  params.set_coeff_modulus(coeff_mod);
  params.set_plain_modulus(plain_mod);
  return {params, avg_mul_ng};
}

vector<int> split_coeff_mod_lowest_nb_primes(int coeff_mod_data_level_size)
{
  int lowest_nb_data_level_primes = coeff_mod_data_level_size / MAX_MOD_SIZE;
  vector<int> coeff_mod_primes_sizes(lowest_nb_data_level_primes, MAX_MOD_SIZE);
  if (coeff_mod_data_level_size % MAX_MOD_SIZE)
  {
    ++lowest_nb_data_level_primes;
    coeff_mod_primes_sizes.push_back(MAX_MOD_SIZE);
    // Remove exceeding bits
    for (int i = 0; i < MAX_MOD_SIZE - (coeff_mod_data_level_size % MAX_MOD_SIZE); ++i)
      --coeff_mod_primes_sizes[i % coeff_mod_primes_sizes.size()];
  }
  // Add special prime
  coeff_mod_primes_sizes.push_back(coeff_mod_primes_sizes.back());
  return coeff_mod_primes_sizes;
}

size_t get_poly_mod(int coeff_mod_total_size, size_t init_value, sec_level_type sec_level)
{
  size_t poly_mod = init_value;
  int max_total_size = CoeffModulus::MaxBitCount(poly_mod, sec_level);
  if (max_total_size < coeff_mod_total_size)
  {
    // Increase poly_mod
    do
    {
      poly_mod <<= 1;
      max_total_size = CoeffModulus::MaxBitCount(poly_mod, sec_level);
      if (max_total_size == 0)
        throw invalid_argument(OUT_HE_STD);
    } while (max_total_size < coeff_mod_total_size);
  }
  else
  {
    // Reduce poly_mod
    while (CoeffModulus::MaxBitCount(poly_mod >> 1, sec_level) >= coeff_mod_total_size)
      poly_mod >>= 1;
  }
  return poly_mod;
}

vector<int> test_params(const SEALContext &context, int xdepth)
{
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
  vector<uint64_t> random_data(slot_count);
  random_device rd;
  for (size_t i = 0; i < slot_count; ++i)
    random_data[i] = context.first_context_data()->parms().plain_modulus().reduce(rd());

  Plaintext plain(context.first_context_data()->parms().poly_modulus_degree(), 0);
  batch_encoder.encode(random_data, plain);
  Ciphertext cipher(context);
  encryptor.encrypt(plain, cipher);

  vector<int> noise_budgets(xdepth + 1, 0);
  noise_budgets[0] = decryptor.invariant_noise_budget(cipher);
  if (noise_budgets[0] == 0)
  {
    // cout << "Budget progress: " << endl;
    // print_noise_budget_progress(noise_budgets, true);
    throw invalid_argument("insufficient noise budget");
  }
  for (int i = 0; i < xdepth; ++i)
  {
    evaluator.multiply_inplace(cipher, cipher);
    evaluator.relinearize_inplace(cipher, relin_keys);
    int noise_budget = decryptor.invariant_noise_budget(cipher);
    if (noise_budget == 0)
    {
      // cout << "Budget progress: " << endl;
      // print_noise_budget_progress(noise_budgets, true);
      throw invalid_argument("insufficient noise budget");
    }
    noise_budgets[i + 1] = noise_budget;
  }
  return noise_budgets;
}

Modulus create_plain_mod(size_t poly_mod, int plain_mod_init_size)
{
  do
  {
    try
    {
      return PlainModulus::Batching(poly_mod, plain_mod_init_size);
    }
    // Suitable prime could not be found
    catch (logic_error &e)
    {
      ++plain_mod_init_size;
    }
  } while (true);
}

void reduce_coeff_mod_size(vector<int> &primes_sizes, int &total_size, int amount)
{
  // Calculate data level bit count
  int data_level_total_size = total_size - primes_sizes.back();
  // Remove special prime size
  primes_sizes.pop_back();
  // Find the first bigger prime index
  int idx = (last_small_prime_index(primes_sizes) + 1) % primes_sizes.size();
  // Reduce data level primes bit sizes by amount starting from idx
  for (int i = 0; i < amount; ++i)
    --primes_sizes[(idx + i) % primes_sizes.size()];
  data_level_total_size -= amount;
  // Add special prime
  primes_sizes.push_back(primes_sizes.back());
  // Update total_size
  total_size = data_level_total_size + primes_sizes.back();
}

int last_small_prime_index(const vector<int> &primes_sizes)
{
  int idx = 0;
  while (idx < primes_sizes.size() - 1 && primes_sizes[idx] == primes_sizes[idx + 1])
    ++idx;
  return idx;
}

void print_noise_budget_progress(const vector<int> &noise_budgets, bool verbose)
{
  cout << "Fresh budget: " << noise_budgets[0] << endl;
  if (noise_budgets.size() == 1)
    return;

  int min_mul_ng = noise_budgets[0] - noise_budgets[1];
  int max_mul_ng = min_mul_ng;
  int nb_muls = noise_budgets.size() - 1;
  for (int i = 1; i <= nb_muls; ++i)
  {
    int mul_ng = noise_budgets[i - 1] - noise_budgets[i];
    if (mul_ng < min_mul_ng)
      min_mul_ng = mul_ng;
    if (mul_ng > max_mul_ng)
      max_mul_ng = mul_ng;
    if (verbose)
      cout << i << " seqmul: " << noise_budgets[i] << " (-" << mul_ng << ")" << endl;
  }
  if (!verbose)
    cout << "Remaining budget: " << noise_budgets.back() << endl;
  cout << "Min growth: " << min_mul_ng << endl;
  cout << "Max growth: " << max_mul_ng << endl;
  int total_ng = noise_budgets[0] - noise_budgets.back();
  cout << "Avg growth: " << static_cast<float>(total_ng) / nb_muls << endl;
}

void print_parameters(const seal::SEALContext &context)
{
  auto &context_data = *context.key_context_data();

  /*
  Which scheme are we using?
  */
  std::string scheme_name;
  switch (context_data.parms().scheme())
  {
  case seal::scheme_type::bfv:
    scheme_name = "BFV";
    break;
  case seal::scheme_type::ckks:
    scheme_name = "CKKS";
    break;
  case seal::scheme_type::bgv:
    scheme_name = "BGV";
    break;
  default:
    throw std::invalid_argument("unsupported scheme");
  }
  std::cout << "/" << std::endl;
  std::cout << "| Encryption parameters :" << std::endl;
  std::cout << "|   scheme: " << scheme_name << std::endl;
  size_t poly_mod = context_data.parms().poly_modulus_degree();
  std::cout << "|   poly_mod: " << poly_mod << " (" << util::get_significant_bit_count(poly_mod) << " bits)"
            << std::endl;

  /*
  Print the size of the true (product) coefficient modulus.
  */
  std::cout << "|   coeff_mod size: ";
  auto coeff_mod = context_data.parms().coeff_modulus();
  int total_bit_count = context_data.total_coeff_modulus_bit_count();
  std::cout << total_bit_count << " (" << total_bit_count - coeff_mod.back().bit_count() << " + "
            << coeff_mod.back().bit_count() << ") (";
  std::size_t coeff_mod_size = coeff_mod.size();
  for (std::size_t i = 0; i < coeff_mod_size - 1; ++i)
  {
    std::cout << coeff_mod[i].bit_count() << " + ";
  }
  std::cout << coeff_mod.back().bit_count();
  std::cout << ") bits" << std::endl;

  /*
  For BFV and BGV schemes print the plain_mod parameter.
  */
  if (
    context_data.parms().scheme() == seal::scheme_type::bfv || context_data.parms().scheme() == seal::scheme_type::bgv)
  {
    Modulus plain_mod = context_data.parms().plain_modulus();
    std::cout << "|   plain_mod: " << plain_mod.value() << " (" << plain_mod.bit_count() << " bits)" << std::endl;
  }

  std::cout << "\\" << std::endl;
}
