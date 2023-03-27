#include "params_simulation.hpp"
#include <fstream>
#include <iostream>
#include <string>

using namespace std;
using namespace seal;

void log_noise_budget_progress(const vector<int> &noise_budgets, ofstream &ofile)
{
  for (int i = 0; i < noise_budgets.size(); ++i)
  {
    ofile << noise_budgets[i] << " ";
    if (i > 0)
      cout << noise_budgets[i - 1] - noise_budgets[i] << " ";
  }
  ofile << endl;
  cout << endl;
}

int main(int argc, char **argv)
{
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
  ifstream params_file("saved_params/" + id + ".params", ios::binary);
  EncryptionParameters params;
  params.load(params_file);
  params_file.close();
  SEALContext context(params, true, sec_level_type::tc128);
  print_parameters(context);
  cout << endl;
  vector<int> noise_budgets;

  ofstream ofile;
  ofile.open("noise_budget_logging/" + id + ".txt", ios_base::app);

  int repeat = 100;
  for (int i = 0; i < repeat; ++i)
  {
    try
    {
      noise_budgets = test_params(context, xdepth);
      log_noise_budget_progress(noise_budgets, ofile);
    }
    // Parameters not sufficient for xdepth
    catch (invalid_argument &e)
    {
      ofile << "failed!" << endl;
      cout << "failed!" << endl;
      break;
    }
  }
  ofile.close();

  return 0;
}
