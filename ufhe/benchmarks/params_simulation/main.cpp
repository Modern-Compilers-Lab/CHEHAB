#include "utils.hpp"
#include <cstddef>
#include <random>
#include <vector>

using namespace std;
using namespace seal;

void test_params(EncryptionParameters params, int xdepth)
{
  SEALContext context(params);
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
  /*
  The number of inserted mod_switch operations is the number of inner coefficients of coeff_modulus, ie without the
  first (key level special modulus) and last (lowest level) modulus.
 */
  int nb_mod_switch = params.coeff_modulus().size() - 2;
  cout << "init noise bud " << decryptor.invariant_noise_budget(encrypted) << endl;
  for (int i = 0; i < xdepth; ++i)
  {
    evaluator.multiply_inplace(encrypted, encrypted);
    if (decryptor.invariant_noise_budget(encrypted) == 0)
      throw invalid_argument("insufficient noise budget");
    evaluator.relinearize_inplace(encrypted, relin_keys);
    cout << "noise bud " << i << " " << decryptor.invariant_noise_budget(encrypted) << endl;
    /*
    // Do mod_switch operation nb_mod_switch times evenly well distributed across xdepth consecutive multiplications
    if (nb_mod_switch && i / (xdepth / nb_mod_switch) != (i + 1) / (xdepth / nb_mod_switch))
      evaluator.mod_switch_to_next_inplace(encrypted);*/
  }
  // Print valid parameters
  print_parameters(context);
}

int main()
{
  scheme_type scheme = scheme_type::bfv;
  sec_level_type sec_level = sec_level_type::tc128;
  int plain_modulus_size = 20;
  int xdepth = 3;
  size_t initial_pmd = 1024;
  int initial_pmd_bit_size = 11;
  size_t max_pmd = 32768;

  size_t poly_modulus_degree = initial_pmd;
  int max_bit_count = CoeffModulus::MaxBitCount(poly_modulus_degree);

  auto increase_pmd = [&]() {
    if (poly_modulus_degree >= max_pmd)
      throw logic_error("parameters for this xdepth could not be selected using the security standard");
    poly_modulus_degree *= 2;
    max_bit_count = CoeffModulus::MaxBitCount(poly_modulus_degree);
  };

  vector<Modulus> coeff_modulus;
  // Initial bit_sizes: one data level prime, special modulus
  vector<int> bit_sizes{initial_pmd_bit_size, initial_pmd_bit_size};
  int bit_count = 2 * initial_pmd_bit_size;
  Modulus plain_modulus;

  do
  {
    try
    {
      cout << poly_modulus_degree << " " << max_bit_count << " ";
      cout << bit_count << " = ";
      for (size_t i = 0; i < bit_sizes.size(); ++i)
      {
        cout << bit_sizes[i] << " + ";
      }
      cout << endl;
      if (bit_count < max_bit_count)
      {
        int avg_bit_size = bit_count / bit_sizes.size();
        if (avg_bit_size < 60)
        {
          for (int i = bit_sizes.size() - 1; i >= 0; --i)
          {
            if (bit_sizes[i] <= avg_bit_size)
            {
              // All primes have the same size
              if (i == bit_sizes.size() - 1)
              {
                // Not to increment only special modulus size making it bigger than other primes
                if (max_bit_count - bit_count > 1)
                {
                  ++bit_sizes[i];
                  ++bit_sizes[i - 1];
                  bit_count += 2;
                }
                else
                  increase_pmd();
              }
              else
              {
                ++bit_sizes[i];
                ++bit_count;
              }
              break;
            }
          }
        }
        else
        {
          int new_bit_count = bit_count - bit_count % (bit_sizes.size() + 1);
          vector<int> new_bit_sizes(bit_sizes.size() + 1, new_bit_count / (bit_sizes.size() + 1));
          int bit_diff = bit_count - new_bit_count;
          // Restore bit_count with bit_sizes.size() + 1 primes and move forward (+1)
          if (bit_diff)
          {
            for (int i = 0; i < bit_diff + 1; ++i)
            {
              ++new_bit_sizes.end()[-1 - i];
              ++new_bit_count;
            }
          }
          else
          {
            // Not to increment only special modulus size making it bigger than other primes
            if (max_bit_count - bit_count > 1)
            {
              ++new_bit_sizes.end()[-1];
              ++new_bit_sizes.end()[-2];
              new_bit_count += 2;
            }
            else
              increase_pmd();
          }
          bit_sizes = new_bit_sizes;
          bit_count = new_bit_count;
        }
      }
      else
        increase_pmd();
      coeff_modulus = CoeffModulus::Create(poly_modulus_degree, bit_sizes);
      // Set plain_modulus in a similar way to PlainModulus::Batching
      plain_modulus = PlainModulus::Batching(poly_modulus_degree, plain_modulus_size);
      // Test selected parameters
      EncryptionParameters params(scheme);
      params.set_poly_modulus_degree(poly_modulus_degree);
      params.set_coeff_modulus(coeff_modulus);
      params.set_plain_modulus(plain_modulus);
      test_params(params, xdepth);
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
  } while (true);

  return 0;
}
