#include "seal/seal.h"
#include <cstddef>
#include <iostream>
#include <random>
#include <stdexcept>
#include <vector>

using namespace std;
using namespace seal;

EncryptionParameters bfv_params_heuristic(
  int initial_plain_m_size, int xdepth, sec_level_type sec_level, bool use_least_levels, size_t initial_poly_md)
{
  if (initial_plain_m_size > 60)
    throw invalid_argument("plain modulus size too large");
  int plain_m_size = initial_plain_m_size;

  size_t max_poly_md = 32768;
  if (initial_poly_md > max_poly_md)
    throw invalid_argument("polynomial modulus too large, not included in fhe security standard");
  size_t poly_modulus_degree = initial_poly_md;

  vector<Modulus> coeff_modulus;
  Modulus plain_modulus;

  // coeff_modulus data level (without special prime) bit count estimation
  int max_fresh_noise = 10;
  int poly_md_bit_count = util::get_significant_bit_count(poly_modulus_degree);
  int worst_case_mul_ng = poly_md_bit_count * 2 + plain_m_size + 1;
  int best_case_mul_ng = max(poly_md_bit_count, plain_m_size);
  int avg_mul_ng = (worst_case_mul_ng + best_case_mul_ng) / 2 + (worst_case_mul_ng + best_case_mul_ng) % 2;
  int coeff_m_data_level_bc = plain_m_size + max_fresh_noise + xdepth * avg_mul_ng;
  // Set coeff_modulus primes sizes
  vector<int> coeff_m_bit_sizes;
  if (use_least_levels)
  {
    int lowest_nb_levels = coeff_m_data_level_bc / 60;
    if (coeff_m_data_level_bc % 60)
      ++lowest_nb_levels;
    coeff_m_bit_sizes.assign(lowest_nb_levels, 60);
    // Remove exceeding bits
    for (int i = 0; i < 60 - (coeff_m_data_level_bc % 60); ++i)
      --coeff_m_bit_sizes[i % coeff_m_bit_sizes.size()];
    // Add special prime
    coeff_m_bit_sizes.push_back(coeff_m_bit_sizes.back());
  }
  else
  {
    int nb_primes = xdepth + 1;
    int avg_prime_size = coeff_m_data_level_bc / nb_primes;
    coeff_m_bit_sizes.assign(nb_primes, avg_prime_size);
    // Add remainding bits
    for (int i = 0; i < coeff_m_data_level_bc % nb_primes; ++i)
      ++coeff_m_bit_sizes.end()[-1 - i];
    // Add special prime
    coeff_m_bit_sizes.push_back(coeff_m_bit_sizes.back());
  }

  // If for the used poly_modulus_degree and the given security level, coeff_modulus size cannot reach the needed value
  if (
    CoeffModulus::MaxBitCount(poly_modulus_degree, sec_level) <
    coeff_m_data_level_bc + ((coeff_m_bit_sizes.size() > 1) ? coeff_m_bit_sizes.back() : 0))
  {
    if (poly_modulus_degree == max_poly_md)
      throw invalid_argument("xdepth too large, corresponding parameters not included in fhe security standard");
    // Repeat with initial n = 2*current_n
    return bfv_params_heuristic(plain_m_size, xdepth, sec_level, use_least_levels, poly_modulus_degree * 2);
  }

  try
  {
    plain_modulus = PlainModulus::Batching(poly_modulus_degree, plain_m_size);
  }
  // suitable prime could not be found
  catch (logic_error &e)
  {
    if (plain_m_size == 60)
      throw invalid_argument("xdepth too large, corresponding parameters not included in fhe security standard");
    //  Repeat with incremented initial plain modulus size
    return bfv_params_heuristic(plain_m_size + 1, xdepth, sec_level, use_least_levels, poly_modulus_degree);
  }

  coeff_modulus = CoeffModulus::Create(poly_modulus_degree, coeff_m_bit_sizes);
  // Create context from the selected parameters
  EncryptionParameters params(scheme_type::bfv);
  params.set_poly_modulus_degree(poly_modulus_degree);
  params.set_coeff_modulus(coeff_modulus);
  params.set_plain_modulus(plain_modulus);
  return params;
}

vector<int> test_params(SEALContext context, int xdepth)
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
  vector<uint64_t> pod_vector;
  random_device rd;
  for (size_t i = 0; i < slot_count; ++i)
    pod_vector.push_back(context.first_context_data()->parms().plain_modulus().reduce(rd()));
  Plaintext plain(context.first_context_data()->parms().poly_modulus_degree(), 0);
  batch_encoder.encode(pod_vector, plain);
  Ciphertext encrypted(context);
  encryptor.encrypt(plain, encrypted);

  vector<int> noise_budgets(xdepth + 1);
  noise_budgets[0] = decryptor.invariant_noise_budget(encrypted);
  if (noise_budgets[0] == 0)
    throw invalid_argument("insufficient noise budget");
  for (int i = 0; i < xdepth; ++i)
  {
    evaluator.multiply_inplace(encrypted, encrypted);
    evaluator.relinearize_inplace(encrypted, relin_keys);
    int noise_budget = decryptor.invariant_noise_budget(encrypted);
    if (noise_budget == 0)
      throw invalid_argument("insufficient noise budget");
    noise_budgets[i + 1] = noise_budget;
  }
  return noise_budgets;
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
  std::cout << "|   poly_modulus_degree: " << context_data.parms().poly_modulus_degree() << std::endl;

  /*
  Print the size of the true (product) coefficient modulus.
  */
  std::cout << "|   coeff_modulus size: ";
  std::cout << context_data.total_coeff_modulus_bit_count() << " (";
  auto coeff_modulus = context_data.parms().coeff_modulus();
  std::size_t coeff_modulus_size = coeff_modulus.size();
  for (std::size_t i = 0; i < coeff_modulus_size - 1; ++i)
  {
    std::cout << coeff_modulus[i].bit_count() << " + ";
  }
  std::cout << coeff_modulus.back().bit_count();
  std::cout << ") bits" << std::endl;

  /*
  For the BFV scheme print the plain_modulus parameter.
  */
  if (context_data.parms().scheme() == seal::scheme_type::bfv)
  {
    std::cout << "|   plain_modulus: " << context_data.parms().plain_modulus().value() << std::endl;
  }

  std::cout << "\\" << std::endl;
}
