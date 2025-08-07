#include "fheco/fheco.hpp"

using namespace std;
using namespace fheco;
#include <chrono>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "../global_variables.hpp" 
/*****************************/ 
void fhe_vectorized(int width){  
  vector<vector<integer>> gx_kernel = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
  vector<vector<integer>> gy_kernel = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}};
  Ciphertext img("img");
  Ciphertext top_row = img >> width;
  Ciphertext bottom_row = img << width;
  ////////////////////
  Ciphertext gx_top_sum =
    gx_kernel[0][0] * (top_row >> 1) + gx_kernel[0][1] * top_row + gx_kernel[0][2] * (top_row << 1);
  Ciphertext gx_curr_sum = gx_kernel[1][0] * (img >> 1) + gx_kernel[1][1] * img + gx_kernel[1][2] * (img << 1);
  Ciphertext gx_bottom_sum =
    gx_kernel[2][0] * (bottom_row >> 1) + gx_kernel[2][1] * bottom_row + gx_kernel[2][2] * (bottom_row << 1);
  Ciphertext gx_result = gx_top_sum + gx_curr_sum + gx_bottom_sum;
  ///////////////////
  Ciphertext gy_top_sum =
    gy_kernel[0][0] * (top_row >> 1) + gy_kernel[0][1] * top_row + gy_kernel[0][2] * (top_row << 1);
  Ciphertext gy_curr_sum = gy_kernel[1][0] * (img >> 1) + gy_kernel[1][1] * img + gy_kernel[1][2] * (img << 1);
  Ciphertext gy_bottom_sum =
    gy_kernel[2][0] * (bottom_row >> 1) + gy_kernel[2][1] * bottom_row + gy_kernel[2][2] * (bottom_row << 1);
  Ciphertext gy_result = gy_top_sum + gy_curr_sum + gy_bottom_sum;
  // combine
  Ciphertext result = gx_result * gx_result + gy_result * gy_result;
  result.set_output("result");
}
/**********************************************************************************/
using Matrix = std::vector<std::vector<Ciphertext>>;

void fhe(int width){
  vector<vector<integer>> gx_kernel = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
  vector<vector<integer>> gy_kernel = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}};
  std::vector<std::vector<Ciphertext>> img = std::vector<std::vector<Ciphertext>>(width, std::vector<Ciphertext>(width));
  std::vector<std::vector<Ciphertext>> gx_output(width, std::vector<Ciphertext>(width));
  std::vector<std::vector<Ciphertext>> gy_output(width, std::vector<Ciphertext>(width));
  std::vector<std::vector<Ciphertext>> output(width, std::vector<Ciphertext>(width));
  std::cout<<"welcome in fhe \n";
  int height = width;
  for (int i = 0; i < width; i++)
  {
    for (int j = 0; j < width; j++)
    {
      img[i][j] = Ciphertext("in_" + std::to_string(i)+"_"+ std::to_string(j));
    } 
  }
  Matrix padded_image(height + 2, std::vector<Ciphertext>(width + 2, Ciphertext(Plaintext(0))));
  for (int i = 0; i < height; ++i)
    for (int j = 0; j < width; ++j)
      padded_image[i + 1][j + 1] = img[i][j];

  // Traverse each pixel in the output image
  for (int i = 1; i <= height; ++i) {
    for (int j = 1; j <= width; ++j) {
        Ciphertext top_sum = padded_image[i - 1][j - 1] * (-1) + padded_image[i - 1][j] * (-2) + padded_image[i - 1][j + 1] * (-1);
        Ciphertext bottom_sum = padded_image[i + 1][j - 1] * 1 + padded_image[i + 1][j] * 2 + padded_image[i + 1][j + 1] * 1;
        gy_output[i - 1][j - 1] = top_sum + bottom_sum;
    }
  }
  /*******************************************************************/
  for (int i = 1; i <= height; ++i) {
    for (int j = 1; j <= width; ++j) {
        Ciphertext top_sum = padded_image[i - 1][j - 1] * 1 + padded_image[i - 1][j + 1] * 1;
        Ciphertext curr_sum = padded_image[i][j - 1] * 2 + padded_image[i][j + 1] * 2 ;
        Ciphertext bottom_sum = padded_image[i + 1][j - 1] * 1 + padded_image[i + 1][j + 1] * 1;
        gx_output[i - 1][j - 1] = top_sum + curr_sum + bottom_sum;
    }
  }
  /*******************************************************************/
  for (int i = 0; i < height; i++)
  {
    for (int j = 0; j < width; j++)
    {
      output[i][j]= gx_output[i][j] * gx_output[i][j] + gy_output[i][j] * gy_output[i][j];
      output[i][j].set_output("out_" + std::to_string(i)+"_"+ std::to_string(j));
    }
  }
}
/*******************************************************************************************/
/*******************************************************************************************/
void print_bool_arg(bool arg, const string &name, ostream &os)
{
  os << (arg ? name : "no_" + name);
}
int main(int argc, char **argv)
{
  bool vectorize_code = true;
  if (argc > 1)
    vectorize_code = stoi(argv[1]);
  
  int slot_count = 1 ;
  if (argc > 2)
    slot_count = stoi(argv[2]);

  int optimization_method = 0;  // 0 = egraph (default), 1 = RL
  if (argc > 3)
    optimization_method = stoi(argv[3]); 

  int window = 0;
  if (argc > 4) 
    window = stoi(argv[4]);

  bool call_quantifier = true;
  if (argc > 5)
    call_quantifier = stoi(argv[5]);

  bool cse = true;
  if (argc > 6)
    cse = stoi(argv[6]);
   
  bool const_folding = true; 
  if (argc > 7)
    const_folding = stoi(argv[7]); 


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
  Compiler::enable_auto_enc_params_selection();
  chrono::high_resolution_clock::time_point t;
  chrono::duration<double, milli> elapsed;
  string func_name = "fhe";
  /**************/t = chrono::high_resolution_clock::now();
  if (vectorize_code)
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
      /********** vectorization Part *******************************/
      if(VECTORIZATION_ENABLED){
        Compiler::gen_vectorized_code(func, window,optimization_method);  // add a flag to specify if the benchmark is structured or no
      }
      /********** Simplification & depth reduction Part ************/
      if(SIMPLIFICATION_ENABLED){
        auto ruleset = Compiler::Ruleset::depth;
        auto rewrite_heuristic = trs::RewriteHeuristic::bottom_up;
        Compiler::compile(func, ruleset, rewrite_heuristic, header_os, gen_name + ".hpp", source_os);
      }
      /********** FHE code generation  *****************************/
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
      const auto &func = Compiler::create_func(func_name,slot_count*slot_count, 20, false, true);
      // update io file 
      std::string updated_inputs_file_name = "fhe_io_example_adapted.txt" ;
      std::string inputs_file_name = "fhe_io_example.txt";
      util::copyFile(inputs_file_name,updated_inputs_file_name);
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
      //auto ruleset = Compiler::Ruleset::depth;
      //auto rewrite_heuristic = trs::RewriteHeuristic::bottom_up;
      ////Compiler::compile(func, ruleset, rewrite_heuristic, header_os, gen_name + ".hpp", source_os);
      auto ruleset = Compiler::Ruleset::simplification_ruleset;
      auto rewrite_heuristic = trs::RewriteHeuristic::bottom_up;
      //Compiler::compile(func, ruleset, rewrite_heuristic, header_os, gen_name + ".hpp", source_os);
      Compiler::gen_he_code(func, header_os, gen_name + ".hpp", source_os, 29);
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