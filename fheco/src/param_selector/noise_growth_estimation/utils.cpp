#include "utils.hpp"
#include <cmath>
#include <fstream>
#include <iostream>
#include <random>
#include <stdexcept>
#include <vector>

using namespace std;
using namespace seal;

EncryptionParameters bfv_no_security_params_heuristic(size_t poly_mod, int init_plain_mod_size, int xdepth)
{
  Modulus plain_mod = create_plain_mod(poly_mod, init_plain_mod_size);
  // coeff_mod data level size estimation (without special prime size)
  int max_fresh_noise = 10;
  int poly_mod_size = util::get_significant_bit_count(poly_mod);
  int plain_mod_size = plain_mod.bit_count();
  int worst_case_mul_ng = poly_mod_size * 2 + plain_mod_size + 1;
  int best_case_mul_ng = max(poly_mod_size, plain_mod_size);
  // Calculate the average and round to the next integer
  float avg_mul_ng = static_cast<float>(worst_case_mul_ng + best_case_mul_ng) / 2;
  int coeff_mod_data_level_size = plain_mod_size + max_fresh_noise + static_cast<int>(ceilf(xdepth * avg_mul_ng));

  // Set coeff_mod primes sizes (use the lowest number of primes)
  int nb_data_level_primes = coeff_mod_data_level_size / MOD_BIT_COUNT_MAX;
  if (coeff_mod_data_level_size % MOD_BIT_COUNT_MAX)
    ++nb_data_level_primes;
  // Add special prime
  vector<int> coeff_mod_primes_sizes(nb_data_level_primes + 1, MOD_BIT_COUNT_MAX);

  // Create coeff modulus primes using coeff_mod_primes_sizes
  vector<Modulus> coeff_mod;
  // If there is a prime in coeff_mod having the same size as plain_mod, we will have coprimality issue
  if (plain_mod.bit_count() == MOD_BIT_COUNT_MAX)
  {
    vector<int> all_mod_primes_sizes(coeff_mod_primes_sizes);
    // Add plain_mod_size
    all_mod_primes_sizes.push_back(MOD_BIT_COUNT_MAX);
    coeff_mod = CoeffModulus::Create(poly_mod, all_mod_primes_sizes);
    plain_mod = coeff_mod.back();
    coeff_mod.pop_back();
  }
  else
    coeff_mod = CoeffModulus::Create(poly_mod, coeff_mod_primes_sizes);
  EncryptionParameters params(scheme_type::bfv);
  params.set_poly_modulus_degree(poly_mod);
  params.set_plain_modulus(plain_mod);
  params.set_coeff_modulus(coeff_mod);
  return params;
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

void estimate_noise_growth_bfv(const SEALContext &context, int xdepth, OpsNoiseGrowth &noise_estimates)
{
  BatchEncoder batch_encoder(context);
  KeyGenerator keygen(context);
  const SecretKey &secret_key = keygen.secret_key();
  PublicKey public_key;
  keygen.create_public_key(public_key);
  RelinKeys relin_keys;
  keygen.create_relin_keys(relin_keys);
  GaloisKeys galois_keys;
  keygen.create_galois_keys(vector<int>{1}, galois_keys);
  Encryptor encryptor(context, public_key);
  Evaluator evaluator(context);
  Decryptor decryptor(context, secret_key);

  auto &context_data = *context.first_context_data();

  int init_noise_budget =
    context_data.total_coeff_modulus_bit_count() - context_data.parms().plain_modulus().bit_count();

  size_t slot_count = batch_encoder.slot_count();
  vector<uint64_t> random_data(slot_count);
  random_device rd;
  for (size_t i = 0; i < slot_count; ++i)
    random_data[i] = context_data.parms().plain_modulus().reduce(rd());

  Plaintext plain;
  batch_encoder.encode(random_data, plain);
  Ciphertext cipher;
  encryptor.encrypt(plain, cipher);
  Ciphertext tmp;

  noise_estimates.encrypt = init_noise_budget - get_invariant_noise_budget(decryptor, cipher);
  int prev_noise_budget = init_noise_budget - noise_estimates.encrypt;
  for (int i = 0; i < xdepth; ++i)
  {
    int noise_growth;

    // mul_plain
    evaluator.multiply_plain(cipher, plain, tmp);
    noise_growth = prev_noise_budget - get_invariant_noise_budget(decryptor, tmp);
    noise_estimates.mul_plain = max(noise_estimates.mul_plain, noise_growth);

    // add
    evaluator.add(cipher, cipher, tmp);
    noise_growth = prev_noise_budget - get_invariant_noise_budget(decryptor, tmp);
    noise_estimates.add = max(noise_estimates.add, noise_growth);

    // add_plain
    evaluator.add_plain(cipher, plain, tmp);
    noise_growth = prev_noise_budget - get_invariant_noise_budget(decryptor, tmp);
    noise_estimates.add_plain = max(noise_estimates.add_plain, noise_growth);

    // rotate
    evaluator.rotate_rows(cipher, 1, galois_keys, tmp);
    noise_growth = prev_noise_budget - get_invariant_noise_budget(decryptor, tmp);
    noise_estimates.rotate = max(noise_estimates.rotate, noise_growth);

    // mul
    evaluator.multiply_inplace(cipher, cipher);
    noise_growth = prev_noise_budget - get_invariant_noise_budget(decryptor, cipher);
    noise_estimates.mul = max(noise_estimates.mul, noise_growth);
    prev_noise_budget = prev_noise_budget - noise_growth;

    // relin
    evaluator.relinearize_inplace(cipher, relin_keys);
    noise_growth = prev_noise_budget - get_invariant_noise_budget(decryptor, cipher);
    noise_estimates.relin = max(noise_estimates.relin, noise_growth);
    prev_noise_budget = prev_noise_budget - noise_growth;
  }
}

void analyze_mod_switch_impact_on_noise_budget(const SEALContext &context, vector<int> &budget_loss_per_noise)
{
  BatchEncoder batch_encoder(context);
  KeyGenerator keygen(context);
  const SecretKey &secret_key = keygen.secret_key();
  PublicKey public_key;
  keygen.create_public_key(public_key);
  Encryptor encryptor(context, public_key);
  Evaluator evaluator(context);
  Decryptor decryptor(context, secret_key);

  auto &context_data = *context.first_context_data();
  int init_noise_budget =
    context_data.total_coeff_modulus_bit_count() - context_data.parms().plain_modulus().bit_count();

  size_t slot_count = batch_encoder.slot_count();
  vector<uint64_t> random_data(slot_count);
  random_device rd;
  for (size_t i = 0; i < slot_count; ++i)
    random_data[i] = context_data.parms().plain_modulus().reduce(rd());

  Plaintext plain;
  batch_encoder.encode(random_data, plain);
  Ciphertext cipher;
  encryptor.encrypt(plain, cipher);
  Ciphertext tmp;

  int curr_noise_budget = get_invariant_noise_budget(decryptor, cipher);
  int fresh_noise = init_noise_budget - curr_noise_budget;
  for (int i = 0; i < budget_loss_per_noise.size(); ++i)
  {
    evaluator.mod_switch_to_next(cipher, tmp);

    int noise = init_noise_budget - curr_noise_budget;
    int budget_after_mod_switch = get_invariant_noise_budget(decryptor, tmp);
    budget_loss_per_noise[noise - fresh_noise] =
      min(budget_loss_per_noise[noise - fresh_noise], budget_after_mod_switch - curr_noise_budget);

    evaluator.add_inplace(cipher, cipher);
    curr_noise_budget = get_invariant_noise_budget(decryptor, cipher);
  }
}

int get_invariant_noise_budget(Decryptor &decryptor, const Ciphertext &cipher)
{
  int inv_noise_budget = decryptor.invariant_noise_budget(cipher);
  if (inv_noise_budget == 0)
    throw logic_error("insufficient initial noise budget");

  return inv_noise_budget;
}

void serialize_bfv_noise_experiments(
  const bfv_noise_experiments_map &bfv_noise_experiments, int repeat, int xdepth, const string &file_name)
{
  ofstream file(file_name);
  file << "repeat " << repeat << ", xdepth " << xdepth << endl;
  file << "{" << endl;
  for (const auto &per_plain_mod_estimates : bfv_noise_experiments)
  {
    file << "{" << per_plain_mod_estimates.first << ",{";
    for (const auto &noise_estimates : per_plain_mod_estimates.second)
    {
      file << "{" << noise_estimates.first << ",{";
      file << noise_estimates.second.encrypt << "," << noise_estimates.second.mul << ","
           << noise_estimates.second.mul_plain;
      file << "}}";
      if (noise_estimates.first != per_plain_mod_estimates.second.rbegin()->first)
        file << ",";
    }
    file << "}}" << endl;
    if (per_plain_mod_estimates.first != bfv_noise_experiments.rbegin()->first)
      file << ",";
  }
  file << "}" << endl;
}

void print_parameters(const SEALContext &context)
{
  auto &context_data = *context.key_context_data();

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

  if (
    context_data.parms().scheme() == seal::scheme_type::bfv || context_data.parms().scheme() == seal::scheme_type::bgv)
  {
    Modulus plain_mod = context_data.parms().plain_modulus();
    std::cout << "|   plain_mod: " << plain_mod.value() << " (" << plain_mod.bit_count() << " bits)" << std::endl;
  }

  std::cout << "\\" << std::endl;
}
