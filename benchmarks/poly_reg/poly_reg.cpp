#include "fheco/fheco.hpp"

using namespace std; 
using namespace fheco;
#include <chrono>
#include <fstream>
#include <iostream> 
#include <string> 
#include <vector>
#include "../global_variables.hpp" 
/***************************/
void fhe_vectorized(int slot_count){ 
  Ciphertext c0("c0"); 
  Ciphertext c1("c1");
  Ciphertext c2("c2"); 
  Ciphertext c3("c3");
  Ciphertext c4("c4");
  Ciphertext c_result = c1 - (c0 * c0 * c4 + c0 * c3 + c2);
  c_result.set_output("c_result");
}
/*******************************************/
void fhe(int slot_count)
{
    size_t size = slot_count;
    std::vector<Ciphertext> v0(size);
    std::vector<Ciphertext> v1(size);
    std::vector<Ciphertext> v2(size);
    std::vector<Ciphertext> v3(size);
    std::vector<Ciphertext> v4(size);
    std::vector<Ciphertext> output(size);
    for (int i = 0; i < size; i++)
    {
      v0[i] = Ciphertext("c0_" + std::to_string(i));
    }
    for (int i = 0; i < size; i++)
    {
      v1[i] = Ciphertext("c1_" + std::to_string(i));
    }
    for (int i = 0; i < size; i++)
    {
      v2[i] = Ciphertext("c2_" + std::to_string(i));
    }
    for (int i = 0; i < size; i++)
    {
      v3[i] = Ciphertext("c3_" + std::to_string(i));
    }

    for (int i = 0; i < size; i++)
    {
      v4[i] = Ciphertext("c4_" + std::to_string(i));
      output[i] = v1[i] + (v4[i] * v0[i] * v0[i] + v3[i] * v0[i] + v2[i]);
    }

    for (int i = 0; i < size; i++)
    {
      output[i].set_output("c_result_" + std::to_string(i));
    }
}
/*************/
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
  //Compiler::enable_auto_enc_params_selection();
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
    Compiler::gen_vectorized_code(func, window, optimization_method);
    auto ruleset = Compiler::Ruleset::depth;
    auto rewrite_heuristic = trs::RewriteHeuristic::bottom_up;
    //Compiler::compile(func, ruleset, rewrite_heuristic, header_os, gen_name + ".hpp", source_os);
    Compiler::gen_he_code(func, header_os, gen_name + ".hpp", source_os);
    /************/elapsed = chrono::high_resolution_clock::now() - t;
    cout<<"Compile time : \n";
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
    // update_io_file 
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
    auto ruleset = Compiler::Ruleset::simplification_ruleset;
    auto rewrite_heuristic = trs::RewriteHeuristic::bottom_up;
    //Compiler::compile(func, ruleset, rewrite_heuristic, header_os, gen_name + ".hpp", source_os);
    Compiler::gen_he_code(func, header_os, gen_name + ".hpp", source_os);
    /************/elapsed = chrono::high_resolution_clock::now() - t;
    cout<<"Compile time : \n";
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