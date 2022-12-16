#include "params_simulation.hpp"
#include <fstream>
#include <string>

using namespace std;
using namespace seal;

int main()
{
  int initial_plain_m_size = 14;
  int xdepth = 23;
  int safety_margin = 5;
  auto [context, noise_budgets] = bfv_params_simulation(initial_plain_m_size, xdepth, safety_margin);
  EncryptionParameters params = context.key_context_data()->parms();

  string ofile_name =
    "params_" + to_string(initial_plain_m_size) + "_" + to_string(xdepth) + "_" + to_string(safety_margin);
  ofstream ofile(ofile_name, ios::binary);
  params.save(ofile, compr_mode_type::none);

  return 0;
}
