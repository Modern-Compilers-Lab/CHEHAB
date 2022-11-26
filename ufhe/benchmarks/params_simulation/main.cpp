#include "utils.hpp"

int main(int argc, char **argv)
{
  int initial_plain_m_size = 17;
  if (argc > 1)
    initial_plain_m_size = atoi(argv[1]);

  int xdepth = 20;
  if (argc > 2)
    xdepth = atoi(argv[2]);

  bool use_least_levels = false;
  sec_level_type sec_level = sec_level_type::tc128;

  scheme_type scheme = scheme_type::bfv;
  size_t max_poly_md = 32768;
  int safety_margin = xdepth / 2;
  // Set Initial parameters using using bfv_params_heuristic
  EncryptionParameters params = bfv_params_heuristic(initial_plain_m_size, xdepth, sec_level, use_least_levels);
  // Initial coeff_m_bit_sizes
  vector<int> coeff_m_bit_sizes(params.coeff_modulus().size());
  int coeff_m_bit_count = 0;
  for (int i = 0; i < coeff_m_bit_sizes.size(); ++i)
  {
    coeff_m_bit_sizes[i] = params.coeff_modulus()[i].bit_count();
    coeff_m_bit_count += coeff_m_bit_sizes[i];
  }
  // Initial poly_modulus_degree
  size_t poly_modulus_degree = params.poly_modulus_degree();
  int max_coeff_m_bit_count = CoeffModulus::MaxBitCount(poly_modulus_degree, sec_level);
  // Initial plain_modulus
  Modulus plain_modulus = params.plain_modulus();
  int plain_m_size = plain_modulus.bit_count();

  // Lambda function to increase poly_modulus_degree
  auto increase_pmd = [&]() {
    string depth_too_large_msg = "xdepth too large, corresponding parameters not included in fhe security standard";
    if (poly_modulus_degree == max_poly_md)
      throw invalid_argument(depth_too_large_msg);

    poly_modulus_degree *= 2;
    max_coeff_m_bit_count = CoeffModulus::MaxBitCount(poly_modulus_degree, sec_level);
    // Set plain modulus for batching with the new poly_modulus_degree
    plain_modulus = create_plain_modulus(poly_modulus_degree, plain_m_size, 60, depth_too_large_msg);
  };

  do
  {
    try
    {
      // Update EncryptionParameters object params with current parameters
      vector<Modulus> coeff_modulus = CoeffModulus::Create(poly_modulus_degree, coeff_m_bit_sizes);
      params.set_poly_modulus_degree(poly_modulus_degree);
      params.set_coeff_modulus(coeff_modulus);
      params.set_plain_modulus(plain_modulus);
      // Create SEALContext
      SEALContext context(params, true, sec_level);
      // Test
      vector<int> noise_budgets = test_params(context, xdepth);
      // Print the first correct parameters
      cout << "xdepth: " << xdepth << endl;
      print_parameters(context);
      // Print noise budget progress over the computation
      print_noise_budget_progress(noise_budgets);
      // Parameters are not minimal
      if (noise_budgets.back() > safety_margin)
      {
        // Reduce coeff modulus bit count
        reduce_coeff_modulus_bit_count(coeff_m_bit_sizes, coeff_m_bit_count, noise_budgets.back() - safety_margin);
        // Reduce poly_modulus_degree according to the minimal coeff_m_bit_count and sec_level
        while (CoeffModulus::MaxBitCount(poly_modulus_degree >> 2, sec_level) >= coeff_m_bit_count)
          poly_modulus_degree >>= 1;
        // If poly_modulus_degree is reduced
        if (params.poly_modulus_degree() > poly_modulus_degree)
          // Retry to create plain modulus with the user given size
          plain_m_size = initial_plain_m_size;
        params.set_poly_modulus_degree(poly_modulus_degree);
        try
        {
          coeff_modulus = CoeffModulus::Create(poly_modulus_degree, coeff_m_bit_sizes);
        }
        catch (logic_error &e)
        {
          throw logic_error("Minimal parameters coeff_modulus primes could not be found");
        }
        params.set_coeff_modulus(coeff_modulus);
        // Increasing plain_modulus above the found parameters does not guarantee corectness of minimal parameters
        plain_modulus = create_plain_modulus(
          poly_modulus_degree, plain_m_size, params.plain_modulus().bit_count(),
          "Minimal parameters plain_modulus prime could not be found");
        params.set_plain_modulus(plain_modulus);
        context = SEALContext(params, true, sec_level);
        // Test parameters
        try
        {
          noise_budgets = test_params(context, xdepth);
        }
        catch (invalid_argument &e)
        {
          throw logic_error("Minimal parameters not correct, evaluation failed");
        }
        // Print minimal correct parameters
        cout << "-----------------------------------------------------" << endl;
        cout << "Minimal parameters" << endl;
        print_parameters(context);
        // Print noise budget progress over the computation
        print_noise_budget_progress(noise_budgets);
      }
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

    // Increment parameters
    if (coeff_m_bit_count < max_coeff_m_bit_count)
    {
      // Increment coeff modulus data level bit count
      int smallest_prime_idx = last_smallest_prime_index(coeff_m_bit_sizes);
      // Smallest prime size can be incremented
      if (coeff_m_bit_sizes[smallest_prime_idx] < 60)
      {
        // All primes have the same size
        if (smallest_prime_idx == coeff_m_bit_sizes.size() - 1)
        {
          // Not to increment only special prime size
          if (max_coeff_m_bit_count - coeff_m_bit_count > 1)
          {
            ++coeff_m_bit_sizes[smallest_prime_idx];
            ++coeff_m_bit_sizes[smallest_prime_idx - 1];
            coeff_m_bit_count += 2;
          }
          else
            increase_pmd();
        }
        else
        {
          ++coeff_m_bit_sizes[smallest_prime_idx];
          ++coeff_m_bit_count;
        }
      }
      // All prime sizes have reached 60, add a new prime
      else
      {
        int data_level_bc = coeff_m_bit_count - coeff_m_bit_sizes.back();
        // new_bit_sizes with only data level primes sizes
        vector<int> new_bit_sizes(coeff_m_bit_sizes.size(), data_level_bc / coeff_m_bit_sizes.size());
        // Add remaining bits and increment
        int remaining_bits = data_level_bc % new_bit_sizes.size();
        for (int i = 0; i < remaining_bits + 1; ++i)
          ++new_bit_sizes.end()[-1 - i];
        // Add special prime
        new_bit_sizes.push_back(new_bit_sizes.back());
        coeff_m_bit_sizes = new_bit_sizes;
        coeff_m_bit_count = data_level_bc + 1 + new_bit_sizes.back();
      }
    }
    else
      increase_pmd();
  } while (true);

  return 0;
}
