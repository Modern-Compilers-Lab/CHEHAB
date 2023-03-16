#include "utils.hpp"
#include <cstddef>
#include <iostream>

using namespace std;
using namespace seal;

int main()
{
  size_t init_poly_mod = 1024;
  size_t max_poly_mod = 32768;
  int plain_mod_max_size = MOD_BIT_COUNT_MAX;

  int experiment_xdepth = 10;
  int repeat = 10;

  bfv_noise_experiments_map bfv_noise_experiments;

  size_t poly_mod = init_poly_mod;
  while (poly_mod <= max_poly_mod)
  {
    int plain_mod_size = util::get_significant_bit_count(poly_mod) + 1;
    while (plain_mod_size <= plain_mod_max_size)
    {
      EncryptionParameters params = bfv_no_security_params_heuristic(poly_mod, plain_mod_size, experiment_xdepth);
      plain_mod_size = params.plain_modulus().bit_count();
      SEALContext context = SEALContext(params, false, sec_level_type::none);
      NoiseEstimatesValue noise_estimates{0, 0, 0};
      for (int i = 0; i < repeat; ++i)
      {
        NoiseEstimatesValue noise_estimates_run_i = estimate_noise_growth(context, experiment_xdepth);
        noise_estimates.fresh_noise = max(noise_estimates.fresh_noise, noise_estimates_run_i.fresh_noise);
        noise_estimates.mul_noise_growth =
          max(noise_estimates.mul_noise_growth, noise_estimates_run_i.mul_noise_growth);
        noise_estimates.mul_plain_noise_growth =
          max(noise_estimates.mul_plain_noise_growth, noise_estimates_run_i.mul_plain_noise_growth);
      }
      auto plain_mod_estimates_it = bfv_noise_experiments.find(plain_mod_size);
      if (plain_mod_estimates_it == bfv_noise_experiments.end())
        bfv_noise_experiments.insert({plain_mod_size, {{poly_mod, noise_estimates}}});
      else
        plain_mod_estimates_it->second.insert({poly_mod, noise_estimates});

      cout << poly_mod << "," << plain_mod_size << " : " << noise_estimates.fresh_noise << ","
           << noise_estimates.mul_noise_growth << "," << noise_estimates.mul_plain_noise_growth << endl;

      ++plain_mod_size;
    }
    poly_mod = poly_mod << 1;
  }
  serialize_bfv_noise_experiments(bfv_noise_experiments, repeat, experiment_xdepth, "bfv_noise_experiments.txt");
  return 0;
}
