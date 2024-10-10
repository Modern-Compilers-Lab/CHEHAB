#include "fheco/fheco.hpp"

using namespace std;
using namespace fheco;
#include <chrono>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>


/*#define height 3
#define width 3
void fhe(int slot_count)
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
    // cout << endl;
  }

  for (int i = 0; i < size; i++)
  {
    output[i].set_output("output_" + std::to_string(i));
  }
}
*/
void fhe(int width){
  vector<vector<integer>> kernel = {{1, 0, 1}, {2, 0, 2}, {1, 0, 1}};
  Ciphertext img("img");
  Ciphertext top_row = img >> width;
  Ciphertext bottom_row = img << width;
  Ciphertext top_sum =  kernel[0][1] * top_row - kernel[0][0] * (top_row >> 1) + kernel[0][2] * (top_row << 1);
  Ciphertext curr_sum =  kernel[1][1] * img - kernel[1][0] * (img >> 1) + kernel[1][2] * (img << 1);
  Ciphertext bottom_sum = kernel[2][1] * bottom_row - kernel[2][0] * (bottom_row >> 1) + kernel[2][2] * (bottom_row << 1);
  Ciphertext result = top_sum + curr_sum + bottom_sum;
  result.set_output("result"); 
}

void print_bool_arg(bool arg, const string &name, ostream &os)
{
  os << (arg ? name : "no_" + name);
}
int main(int argc, char **argv)
{
  bool vectorized = true;
  if (argc > 1)
    vectorized = stoi(argv[1]);

  int window = 0;
  if (argc > 2) 
    window = stoi(argv[2]);

  bool call_quantifier = true;
  if (argc > 3)
    call_quantifier = stoi(argv[3]);

  bool cse = true;
  if (argc > 4)
    cse = stoi(argv[4]);
  
  int slot_count = 1 ;
  if (argc > 5)
    slot_count = stoi(argv[5]);

  bool const_folding = true;
  if (argc > 5)
    const_folding = stoi(argv[5]);

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
  string func_name = "fhe";
  /**************/t = chrono::high_resolution_clock::now();
  if (vectorized)
  {
      const auto &func = Compiler::create_func(func_name, 1, 20, false, true);
      fhe(slot_count);
      string gen_name = "_gen_he_" + func_name;
      string gen_path = "he/" + gen_name;
      ofstream header_os(gen_path + ".hpp");
      if (!header_os)
        throw logic_error("failed to create header file");
      ofstream source_os(gen_path + ".cpp");
      if (!source_os)
        throw logic_error("failed to create source file");
      cout << " window is " << window << endl;
      Compiler::gen_vectorized_code(func, window);
      Compiler::gen_he_code(func, header_os, gen_name + ".hpp", source_os);
      
      /************/elapsed = chrono::high_resolution_clock::now() - t;
      
      cout << elapsed.count() << " ms\n";
      if (call_quantifier)
      {
        util::Quantifier quantifier{func};
        quantifier.run_all_analysis();
        quantifier.print_info(cout);
      }
  }
  else
  {
      const auto &func = Compiler::create_func(func_name, slot_count, 20, false, true);
      double squareRoot = sqrt(slot_count);
      int width = static_cast<int>(squareRoot);
      fhe(width);
      string gen_name = "_gen_he_" + func_name;
      string gen_path = "he/" + gen_name;
      ofstream header_os(gen_path + ".hpp");
      if (!header_os)
        throw logic_error("failed to create header file");
      ofstream source_os(gen_path + ".cpp");
      if (!source_os)
        throw logic_error("failed to create source file");
      auto ruleset = Compiler::Ruleset::ops_cost;
      auto rewrite_heuristic = trs::RewriteHeuristic::bottom_up;
      Compiler::compile(func, ruleset, rewrite_heuristic, header_os, gen_name + ".hpp", source_os);
      
      /************/elapsed = chrono::high_resolution_clock::now() - t;

      cout << elapsed.count() << " ms\n";
      if (call_quantifier)
      {
        util::Quantifier quantifier{func};
        quantifier.run_all_analysis();
        quantifier.print_info(cout);
      }
  }
  return 0;
}