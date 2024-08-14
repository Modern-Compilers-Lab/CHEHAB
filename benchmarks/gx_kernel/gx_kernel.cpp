#include "fheco/fheco.hpp"

using namespace std;
using namespace fheco;
#include <chrono>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#define height 3
#define width 3

void fhe()
{
  size_t size = (height + 1) * (width + 2) + width + 1 + 6 + 1;
  std::vector<Ciphertext> v(size);
  std::vector<Ciphertext> output(size);
  for (int i = 0; i < size; i++)
  {
    v[i] = Ciphertext("v_" + std::to_string(i));
    output[i] = v[i];
  }
  for (int i = 0; i < height + 2; i++)
  {

    for (int j = 0; j < width + 2; j++)
    {
      int ii = i * (width + 2) + j;
      Ciphertext temp_out = encrypt(0);

      // Top left
      if (ii - 6 > 0)
      {
        temp_out -= v[ii - 6];
      }
      // Top right
      if (ii - 4 > 0)
      {
        temp_out += v[ii - 4];
      }

      // Middle left
      if (ii - 1 > 0)
      {
        temp_out = temp_out - 2 * v[ii - 1];
      }
      // Middle right
      temp_out = temp_out + 2 * v[ii + 1];

      // Bottom left
      temp_out = temp_out - v[ii + 4];

      // Bottom right
      temp_out = temp_out + v[ii + 6];
      output[ii] = temp_out;
      // cout << result[ii] << " ";
      // cout << temp_out << " ";
    }

    /*
      you can do something like
      for(int i = 0; i < poly_modulus_degree; i++)
        pt_result[i].set_output("o" + std::string(i))
    */
    // cout << endl;
  }

  for (int i = 0; i < size; i++)
  {
    output[i].set_output("output_" + std::to_string(i));
  }
}

void print_bool_arg(bool arg, const string &name, ostream &os)
{
  os << (arg ? name : "no_" + name);
}
int main(int argc, char **argv)
{
  bool call_quantifier = false;
  if (argc > 1)
    call_quantifier = stoi(argv[1]);

  bool window = 0;
  if (argc > 3)
    window = stoi(argv[2]);

  bool cse = true;
  if (argc > 3)
    cse = stoi(argv[3]);

  bool const_folding = true;
  if (argc > 4)
    const_folding = stoi(argv[4]);

  print_bool_arg(call_quantifier, "quantifier", clog);
  clog << " ";
  print_bool_arg(cse, "cse", clog);
  clog << " ";
  print_bool_arg(const_folding, "constant_folding", clog);
  clog << '\n';

  if (cse)
  {
    Compiler::enable_cse();
    Compiler::enable_order_operands();
  }
  else
  {
    Compiler::disable_cse();
    Compiler::disable_order_operands();
  }

  if (const_folding)
    Compiler::enable_const_folding();
  else
    Compiler::disable_const_folding();

  chrono::high_resolution_clock::time_point t;
  chrono::duration<double, milli> elapsed;
  t = chrono::high_resolution_clock::now();
  string func_name = "fhe";
  const auto &func = Compiler::create_func(func_name, 1, 20, false, true);
  fhe();
  string gen_name = "_gen_he_" + func_name;
  string gen_path = "he/" + gen_name;
  ofstream header_os(gen_path + ".hpp");
  if (!header_os)
    throw logic_error("failed to create header file");

  ofstream source_os(gen_path + ".cpp");
  if (!source_os)
    throw logic_error("failed to create source file");

  Compiler::gen_vectorized_code(func, window);
  elapsed = chrono::high_resolution_clock::now() - t;
  cout << elapsed.count() << " ms\n";

  if (call_quantifier)
  {
    util::Quantifier quantifier{func};
    quantifier.run_all_analysis();
    quantifier.print_info(cout);
  }
  return 0;
}