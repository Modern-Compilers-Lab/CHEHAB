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

int main()
{
  int initial_plain_m_size = 14;
  int xdepth = 23;
  int safety_margin = 23;

  string name_suffix = to_string(initial_plain_m_size) + "_" + to_string(xdepth) + "_" + to_string(safety_margin);

  ofstream ofile;
  ofile.open("noise_budget_logging_" + name_suffix + ".txt", ios_base::app);

  string params_file_name = "params_" + name_suffix;
  ifstream params_file(params_file_name, ios::binary);
  EncryptionParameters params;
  params.load(params_file);
  SEALContext context(params, true, sec_level_type::tc128);
  print_parameters(context);
  cout << endl;
  vector<int> noise_budgets;

  int repeat = 10;
  for (int i = 0; i < repeat; ++i)
  {
    noise_budgets = test_params(context, xdepth);
    log_noise_budget_progress(noise_budgets, ofile);
  }

  return 0;
}
