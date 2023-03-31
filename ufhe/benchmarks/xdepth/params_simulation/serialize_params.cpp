#include "params_simulation.hpp"
#include <fstream>
#include <string>

using namespace std;
using namespace seal;

int main(int argc, char **argv)
{
  EncryptionParameters params(scheme_type::bfv);

  int plain_mod_size = 17;
  if (argc > 1)
    plain_mod_size = atoi(argv[1]);

  int xdepth = 10;
  if (argc > 2)
    xdepth = atoi(argv[2]);

  int coeff_mod_data_level_size = 333;
  if (argc > 3)
    coeff_mod_data_level_size = atoi(argv[3]);

  string id = to_string(plain_mod_size) + "_" + to_string(xdepth) + "_" + to_string(coeff_mod_data_level_size);

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

  string ofile_name = "saved_params/" + id + ".params";
  ofstream ofile(ofile_name, ios::binary);
  params.save(ofile, compr_mode_type::none);
  ofile.close();

  return 0;
}
