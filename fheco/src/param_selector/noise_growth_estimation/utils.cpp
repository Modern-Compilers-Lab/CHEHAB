#include "utils.hpp"
#include <cmath>
#include <fstream>
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
  vector<Modulus> coeff_mod = CoeffModulus::Create(poly_mod, coeff_mod_primes_sizes);
  EncryptionParameters params(scheme_type::bfv);
  params.set_poly_modulus_degree(poly_mod);
  params.set_coeff_modulus(coeff_mod);
  params.set_plain_modulus(plain_mod);
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

NoiseEstimatesValue estimate_noise_growth(const SEALContext &context, int xdepth)
{
  NoiseEstimatesValue noise_estimates{0, 0, 0};

  BatchEncoder batch_encoder(context);
  KeyGenerator keygen(context);
  const SecretKey &secret_key = keygen.secret_key();
  PublicKey public_key;
  keygen.create_public_key(public_key);
  RelinKeys relin_keys;
  keygen.create_relin_keys(relin_keys);
  Encryptor encryptor(context, public_key);
  Evaluator evaluator(context);
  Decryptor decryptor(context, secret_key);

  auto get_invariant_noise_budget = [&decryptor](const Ciphertext &cipher) -> int {
    int inv_noise_budget = decryptor.invariant_noise_budget(cipher);
    if (inv_noise_budget == 0)
      throw logic_error("insufficient initial noise budget");

    return inv_noise_budget;
  };

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

  noise_estimates.fresh_noise = init_noise_budget - get_invariant_noise_budget(cipher);
  int prev_noise_budget = init_noise_budget - noise_estimates.fresh_noise;
  for (int i = 0; i < xdepth; ++i)
  {
    // cipher-plain mul
    evaluator.multiply_plain(cipher, plain, tmp);
    int mul_plain_noise = prev_noise_budget - get_invariant_noise_budget(tmp);
    noise_estimates.mul_plain_noise_growth = max(noise_estimates.mul_plain_noise_growth, mul_plain_noise);

    // cipher-cipher mul
    evaluator.multiply_inplace(cipher, cipher);
    int mul_noise = prev_noise_budget - get_invariant_noise_budget(cipher);
    noise_estimates.mul_noise_growth = max(noise_estimates.mul_noise_growth, mul_noise);

    evaluator.relinearize_inplace(cipher, relin_keys);
    prev_noise_budget = get_invariant_noise_budget(cipher);
  }
  return noise_estimates;
}

void serialize_bfv_noise_experiments(const bfv_noise_experiments_map &bfv_noise_experiments, const string &file_name)
{
  ofstream file(file_name);
  file << "{" << endl;
  for (const auto &per_plain_mod_estimates : bfv_noise_experiments)
  {
    file << "{" << per_plain_mod_estimates.first << ",{";
    for (const auto &noise_estimates : per_plain_mod_estimates.second)
    {
      file << "{" << noise_estimates.first << ",{";
      file << noise_estimates.second.fresh_noise << "," << noise_estimates.second.mul_noise_growth << ","
           << noise_estimates.second.mul_plain_noise_growth;
      file << "}}";
      if (noise_estimates.first != per_plain_mod_estimates.second.rbegin()->first)
        file << ",";
    }
    file << "}}" << endl;
    if (per_plain_mod_estimates.first != bfv_noise_experiments.rbegin()->first)
      file << ",";
  }
  file << "}" << endl;
  file.close();
}
