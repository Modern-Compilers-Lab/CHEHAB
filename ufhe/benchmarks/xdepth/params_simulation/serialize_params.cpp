#include "params_simulation.hpp"
#include <fstream>
#include <string>

using namespace std;
using namespace seal;

int main(int argc, char **argv)
{
  int init_plain_mod_size = 14;
  if (argc > 1)
    init_plain_mod_size = atoi(argv[1]);

  int xdepth = 10;
  if (argc > 2)
    xdepth = atoi(argv[2]);

  int safety_margin = xdepth;
  if (argc > 3)
    safety_margin = atoi(argv[3]);

  auto [context, noise_budgets] = bfv_params_simulation(init_plain_mod_size, xdepth, safety_margin);
  EncryptionParameters params = context.key_context_data()->parms();

  string id =
    to_string(params.plain_modulus().bit_count()) + "_" + to_string(xdepth) + "_" +
    to_string(context.key_context_data()->total_coeff_modulus_bit_count() - params.coeff_modulus().back().bit_count());
  string ofile_name = "saved_params/" + id + ".params";
  ofstream ofile(ofile_name, ios::binary);
  params.save(ofile, compr_mode_type::none);
  ofile.close();

  return 0;
}
