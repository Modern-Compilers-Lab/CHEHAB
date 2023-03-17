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
  int repeat = 1;

  // mod_switch impact on noise budget
  EncryptionParameters params(scheme_type::bfv);
  size_t mod_switch_analysis_poly_mod = 1024;
  params.set_poly_modulus_degree(mod_switch_analysis_poly_mod);
  params.set_plain_modulus(create_plain_mod(mod_switch_analysis_poly_mod, 14));
  params.set_coeff_modulus(CoeffModulus::Create(mod_switch_analysis_poly_mod, vector<int>(4, MOD_BIT_COUNT_MAX)));
  SEALContext context = SEALContext(params, true, sec_level_type::none);
  vector<int> budget_loss_per_noise(2 * MOD_BIT_COUNT_MAX, 1);
  for (int i = 0; i < repeat * 100; ++i)
    analyze_mod_switch_impact_on_noise_budget(context, budget_loss_per_noise);

  for (int i = 0; i < MOD_BIT_COUNT_MAX; ++i)
    cout << budget_loss_per_noise[i] << ",";
  cout << endl;
  for (int i = 0; i < MOD_BIT_COUNT_MAX; ++i)
    cout << budget_loss_per_noise[MOD_BIT_COUNT_MAX + i] << ",";
  cout << endl;

  char c;
  cin >> c;

  // Operations noise growth estimation for different n,t
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
      OpsNoiseGrowth noise_estimates;
      for (int i = 0; i < repeat; ++i)
        estimate_noise_growth_bfv(context, experiment_xdepth, noise_estimates);
      auto plain_mod_estimates_it = bfv_noise_experiments.find(plain_mod_size);
      if (plain_mod_estimates_it == bfv_noise_experiments.end())
        bfv_noise_experiments.insert({plain_mod_size, {{poly_mod, noise_estimates}}});
      else
        plain_mod_estimates_it->second.insert({poly_mod, noise_estimates});

      cout << poly_mod << "," << plain_mod_size << " : " << noise_estimates.encrypt << "," << noise_estimates.mul << ","
           << noise_estimates.mul_plain << "," << noise_estimates.add << "," << noise_estimates.add_plain << ","
           << noise_estimates.rotate << "," << noise_estimates.relin << endl;

      ++plain_mod_size;
    }
    poly_mod = poly_mod << 1;
  }
  serialize_bfv_noise_experiments(bfv_noise_experiments, repeat, experiment_xdepth, "bfv_noise_experiments.txt");
  return 0;
}
