#include "utils.hpp"

EncryptionParameters bfv_params_heuristic(
  int initial_plain_m_size = 17, int xdepth = 23, sec_level_type sec_level = sec_level_type::tc128,
  bool use_least_levels = false, size_t initial_poly_md = 1024);

vector<int> test_params(SEALContext context, int xdepth);

void print_parameters(const seal::SEALContext &context);

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
  int safety_margin = 3;

  EncryptionParameters params = bfv_params_heuristic(initial_plain_m_size, xdepth, sec_level, use_least_levels);

  // Initial coeff_m_bit_sizes using bfv_params_heuristic
  vector<int> coeff_m_bit_sizes(params.coeff_modulus().size());
  int coeff_m_bit_count = 0;
  for (int i = 0; i < coeff_m_bit_sizes.size(); ++i)
  {
    coeff_m_bit_sizes[i] = params.coeff_modulus()[i].bit_count();
    coeff_m_bit_count += coeff_m_bit_sizes[i];
  }
  size_t poly_modulus_degree = params.poly_modulus_degree();
  int max_coeff_m_bit_count = CoeffModulus::MaxBitCount(poly_modulus_degree, sec_level);
  Modulus plain_modulus = params.plain_modulus();
  int plain_m_size = plain_modulus.bit_count();

  auto increase_pmd = [&]() {
    if (poly_modulus_degree == max_poly_md)
      throw invalid_argument("xdepth too large, corresponding parameters not included in fhe security standard");

    poly_modulus_degree *= 2;
    max_coeff_m_bit_count = CoeffModulus::MaxBitCount(poly_modulus_degree, sec_level);
    // Set plain modulus for batching with the new poly_modulus_degree
    do
    {
      try
      {
        plain_modulus = PlainModulus::Batching(poly_modulus_degree, plain_m_size);
        break;
      }
      // suitable prime could not be found
      catch (logic_error &e)
      {
        if (plain_m_size == 60)
          throw invalid_argument("xdepth too large, corresponding parameters not included in fhe security standard");
        ++plain_m_size;
      }
    } while (true);
  };

  do
  {
    try
    {
      vector<Modulus> coeff_modulus = CoeffModulus::Create(poly_modulus_degree, coeff_m_bit_sizes);
      // Test selected parameters
      EncryptionParameters params(scheme);
      params.set_poly_modulus_degree(poly_modulus_degree);
      params.set_coeff_modulus(coeff_modulus);
      params.set_plain_modulus(plain_modulus);
      SEALContext context(params, true, sec_level);
      // Test parameters
      vector<int> noise_budgets = test_params(context, xdepth);
      // Print the first correct parameters
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
      // Parameters are not minimal
      if (noise_budgets.back() > safety_margin)
      {
        // Reduce coeff modulus bit count
        // coeff_m_bit_count -= noise_budgets.back() - safety_margin;
        // Calculate coeff modulus data level bit count
        int data_level_bc = coeff_m_bit_count - coeff_m_bit_sizes.back();
        // Remove special prime size for now
        coeff_m_bit_sizes.pop_back();
        int first_bigger_prime_idx = coeff_m_bit_sizes.size() - 1;
        while (first_bigger_prime_idx > 0 &&
               coeff_m_bit_sizes[first_bigger_prime_idx] == coeff_m_bit_sizes[first_bigger_prime_idx - 1])
          --first_bigger_prime_idx;
        for (int i = 0; i < noise_budgets.back() - safety_margin; ++i)
          --coeff_m_bit_sizes[(first_bigger_prime_idx + i) % coeff_m_bit_sizes.size()];
        data_level_bc -= noise_budgets.back() - safety_margin;
        // Add special prime
        coeff_m_bit_sizes.push_back(coeff_m_bit_sizes.back());
        coeff_m_bit_count = data_level_bc + coeff_m_bit_sizes.back();
        // Reduce poly_modulus_degree according to the minimal coeff_m_bit_count and sec_level
        while (CoeffModulus::MaxBitCount(poly_modulus_degree >> 2, sec_level) >= coeff_m_bit_count)
          poly_modulus_degree >>= 1;
        // If poly_modulus_degree is reduced, retry to create plain modulus with the user given size
        if (params.poly_modulus_degree() > poly_modulus_degree)
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
        do
        {
          try
          {
            plain_modulus = PlainModulus::Batching(poly_modulus_degree, plain_m_size);
            break;
          }
          catch (logic_error &e)
          {
            // Increasing plain_modulus above the found parameters does not guarantee corectness of minimal parameters
            if (plain_m_size == params.plain_modulus().bit_count())
              throw logic_error("Minimal parameters plain_modulus prime could not be found");
            ++plain_m_size;
          }
        } while (true);
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
        for (int i = 0; i < noise_budgets.size(); ++i)
        {
          if (i == 0)
            cout << "Fresh noise budget: " << noise_budgets[0] << endl;
          else
            cout << "After " << i << " sequential muls: " << noise_budgets[i] << endl;
        }
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
