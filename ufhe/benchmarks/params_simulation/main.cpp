#include "utils.hpp"
#include <cstddef>
#include <random>
#include <stdexcept>
#include <vector>

using namespace std;
using namespace seal;

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

int main(int argc, char **argv)
{
  int max_xdepth = 20;
  int min_xdepth = 0;
  int initial_plain_m_size = 17;
  int initial_coeff_m_bit_count = 2 * (initial_plain_m_size + 1);
  if (argc > 1)
    max_xdepth = atoi(argv[1]);
  if (argc > 2)
    min_xdepth = atoi(argv[2]);
  if (argc > 3)
    initial_coeff_m_bit_count = atoi(argv[3]);

  if (min_xdepth > max_xdepth)
    throw invalid_argument("min_xdepth cannot be greater than max_xdepth");

  scheme_type scheme = scheme_type::bfv;
  sec_level_type sec_level = sec_level_type::tc128;
  size_t initial_poly_md = 1024;

  if (initial_plain_m_size < 17)
    throw invalid_argument("plain modulus size must be at least 17 bits to support batching with n up to 32768");

  size_t max_poly_md = 32768;
  int xdepth = min_xdepth;

  vector<Modulus> coeff_modulus;
  // Initial coeff_m_bit_sizes with the user provided initial_coeff_m_bit_count
  vector<int> coeff_m_bit_sizes;
  int coeff_m_bit_count = initial_coeff_m_bit_count;
  int initial_nb_primes = coeff_m_bit_count / 60;
  if (coeff_m_bit_count % 60)
    ++initial_nb_primes;
  // No special prime
  if (initial_nb_primes == 1)
    coeff_m_bit_sizes.assign(2, coeff_m_bit_count / 2);
  else
  {
    coeff_m_bit_sizes.assign(initial_nb_primes, 60);
    for (int i = 0; i < 60 - (coeff_m_bit_count % 60); ++i)
      --coeff_m_bit_sizes[i % coeff_m_bit_sizes.size()];
  }

  size_t poly_modulus_degree = initial_poly_md;
  int max_coeff_m_bit_count = CoeffModulus::MaxBitCount(poly_modulus_degree, sec_level);

  int plain_m_size = initial_plain_m_size;
  Modulus plain_modulus = PlainModulus::Batching(poly_modulus_degree, plain_m_size);

  auto increase_pmd = [&]() {
    if (poly_modulus_degree >= max_poly_md)
      throw logic_error("parameters for this xdepth could not be selected using the security standard");
    poly_modulus_degree *= 2;
    max_coeff_m_bit_count = CoeffModulus::MaxBitCount(poly_modulus_degree, sec_level);
    try
    {
      plain_modulus = PlainModulus::Batching(poly_modulus_degree, plain_m_size);
    }
    // suitable prime could not be found
    catch (logic_error &e)
    {
      ++plain_m_size;
    }
  };

  while (max_coeff_m_bit_count < coeff_m_bit_count)
    increase_pmd();

  do
  {
    try
    {
      coeff_modulus = CoeffModulus::Create(poly_modulus_degree, coeff_m_bit_sizes);
      // Test selected parameters
      EncryptionParameters params(scheme);
      params.set_poly_modulus_degree(poly_modulus_degree);
      params.set_coeff_modulus(coeff_modulus);
      params.set_plain_modulus(plain_modulus);
      SEALContext context(params, false, sec_level);
      // Test parameters
      vector<int> noise_budgets = test_params(context, xdepth);
      // Print correct parameters
      cout << "xdepth= " << xdepth << endl;
      print_parameters(context);
      // Print noise budget progress over the computation
      for (int i = 0; i < noise_budgets.size(); ++i)
      {
        if (i == 0)
          cout << "Fresh noise budget: " << noise_budgets[0] << endl;
        else
          cout << "After " << i << " sequential muls: " << noise_budgets[i] << endl;
      }
      if (xdepth < max_xdepth)
      {
        ++xdepth;
        cout << "-----------------------------------------------------" << endl;
      }
      else
        break;
    }
    catch (invalid_argument &e)
    {
      // Selected parameters offer insufficient noise budget
      // cout << e.what() << endl;
    }
    catch (logic_error &e)
    {
      // Not enough suitable primes could be found
      // cout << e.what() << endl;
    }

    if (coeff_m_bit_count < max_coeff_m_bit_count)
    {
      int avg_bit_size = coeff_m_bit_count / coeff_m_bit_sizes.size();
      if (avg_bit_size < 60)
      {
        for (int i = coeff_m_bit_sizes.size() - 1; i >= 0; --i)
        {
          if (coeff_m_bit_sizes[i] <= avg_bit_size)
          {
            // All primes have the same size
            if (i == coeff_m_bit_sizes.size() - 1)
            {
              // Not to increment only special modulus size making it bigger than other primes
              if (max_coeff_m_bit_count - coeff_m_bit_count > 1)
              {
                ++coeff_m_bit_sizes[i];
                ++coeff_m_bit_sizes[i - 1];
                coeff_m_bit_count += 2;
              }
              else
                increase_pmd();
            }
            else
            {
              ++coeff_m_bit_sizes[i];
              ++coeff_m_bit_count;
            }
            break;
          }
        }
      }
      else
      {
        int new_coeff_m_bit_count = coeff_m_bit_count - coeff_m_bit_count % (coeff_m_bit_sizes.size() + 1);
        vector<int> new_coeff_m_bit_sizes(
          coeff_m_bit_sizes.size() + 1, new_coeff_m_bit_count / (coeff_m_bit_sizes.size() + 1));
        int data_level_bc = coeff_m_bit_count - coeff_m_bit_sizes.back();
        int new_data_level_bc = new_coeff_m_bit_count - new_coeff_m_bit_sizes.back();
        int data_level_bit_diff = new_data_level_bc - data_level_bc;
        if (data_level_bit_diff > 0)
        {
          // Decrease primes sizes to go back to step_size=1
          for (int i = 0; i < data_level_bit_diff - 1; ++i)
          {
            --new_coeff_m_bit_sizes[i % new_coeff_m_bit_sizes.size()];
            --new_coeff_m_bit_count;
          }
        }
        else
        {
          data_level_bit_diff *= -1;
          if (data_level_bit_diff)
          {
            // Increase primes sizes to reach step_size=+1
            for (int i = 0; i < data_level_bit_diff + 1; ++i)
            {
              ++new_coeff_m_bit_sizes.end()[-1 - i];
              ++new_coeff_m_bit_count;
            }
          }
          else
          {
            // Not to increment only special modulus size making it bigger than other primes
            if (max_coeff_m_bit_count - new_coeff_m_bit_count > 1)
            {
              ++new_coeff_m_bit_sizes.end()[-1];
              ++new_coeff_m_bit_sizes.end()[-2];
              new_coeff_m_bit_count += 2;
            }
            else
              increase_pmd();
          }
        }
        coeff_m_bit_sizes = new_coeff_m_bit_sizes;
        coeff_m_bit_count = new_coeff_m_bit_count;
      }
    }
    else
      increase_pmd();
  } while (true);

  return 0;
}
