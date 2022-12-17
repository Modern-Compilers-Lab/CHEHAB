#include "params_simulation.hpp"
#include <fstream>
#include <string>

using namespace std;
using namespace seal;

int main(int argc, char **argv)
{
  EncryptionParameters params(scheme_type::bfv);
  string id;

  bool from_xdepth = false;
  if (argc > 1)
    from_xdepth = atoi(argv[1]);

  if (from_xdepth)
  {
    int init_plain_mod_size = 14;
    if (argc > 2)
      init_plain_mod_size = atoi(argv[2]);

    int xdepth = 10;
    if (argc > 3)
      xdepth = atoi(argv[3]);

    int safety_margin = xdepth;
    if (argc > 4)
      safety_margin = atoi(argv[4]);

    const SEALContext &context = get<0>(bfv_params_simulation(init_plain_mod_size, xdepth, safety_margin));
    auto &context_data = *context.key_context_data();
    params = context_data.parms();
    id = to_string(params.plain_modulus().bit_count()) + "_" + to_string(xdepth) + "_" +
         to_string(context_data.total_coeff_modulus_bit_count() - params.coeff_modulus().back().bit_count());
  }
  else
  {
    int plain_mod_size = 17;
    if (argc > 2)
      plain_mod_size = atoi(argv[2]);

    int coeff_mod_data_level_size = 333;
    if (argc > 3)
      coeff_mod_data_level_size = atoi(argv[3]);

    int xdepth = 10;
    if (argc > 4)
      xdepth = atoi(argv[4]);

    id = to_string(plain_mod_size) + "_" + to_string(xdepth) + "_" + to_string(coeff_mod_data_level_size);

    vector<int> coeff_mod_primes_sizes = split_coeff_mod_lowest_nb_primes(coeff_mod_data_level_size);
    int coeff_mod_total_size = coeff_mod_data_level_size + coeff_mod_primes_sizes.back();
    size_t poly_mod = get_poly_mod(coeff_mod_total_size, 1024);
    vector<Modulus> coeff_mod = CoeffModulus::Create(poly_mod, coeff_mod_primes_sizes);
    Modulus plain_mod = PlainModulus::Batching(poly_mod, plain_mod_size);
    params.set_poly_modulus_degree(poly_mod);
    params.set_coeff_modulus(coeff_mod);
    params.set_plain_modulus(plain_mod);
    SEALContext context(params);
    print_parameters(context);
  }

  string ofile_name = "saved_params/" + id + ".params";
  ofstream ofile(ofile_name, ios::binary);
  params.save(ofile, compr_mode_type::none);
  ofile.close();

  return 0;
}
