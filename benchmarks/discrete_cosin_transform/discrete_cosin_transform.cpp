#include "fheco/fheco.hpp"

using namespace std;
using namespace fheco;
#include <chrono>
#include <fstream>
#include <iostream> 
#include <string>  
#include <vector> 
#include "../global_variables.hpp" 
/********************/
void fhe_vectorized(int slot_count){
  Ciphertext c1("c1");  
  Ciphertext c2("c2"); 
  Ciphertext slot_wise_diff = (c1 - c2) * (c1 - c2);
  Ciphertext sum = SumVec(slot_wise_diff,slot_count);
  sum.set_output("result"); 
} 
/***************************/
void fhe(int slot_count)
{
  Ciphertext x0("x0");  // row[0]
  Ciphertext x1("x1");  // row[1]
  Ciphertext x2("x2");  // row[2]
  Ciphertext x3("x3");  // row[3]
  Plaintext p0("p0"), p1("p1"), p2("p2"), p3("p3");
  // Even-odd butterflies
  Ciphertext s0 = x0 + x3;
  Ciphertext d0 = x0 - x3;
  Ciphertext s1 = x1 + x2;
  Ciphertext d1 = x1 - x2;
  // Apply 1D DCT coefficients (e.g., scaled cosine basis)
  Ciphertext C0 = (s0 + s1) * p0;
  Ciphertext C1 = (d0 * p1) + (d1 * p2);
  Ciphertext C2 = (s0 - s1) * p3;
  Ciphertext C3 = (d0 * p2) - (d1 * p1);

  C0.set_output("output0");
  C1.set_output("output1");
  C2.set_output("output2");
  C3.set_output("output3");
}


/********************************************/
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
    /********** vectorization Part *******************************/
    if(VECTORIZATION_ENABLED){
      Compiler::gen_vectorized_code(func, window, optimization_method);  // add a flag to specify if the benchmark is structured or no
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
    Compiler::compile(func, ruleset, rewrite_heuristic, header_os, gen_name + ".hpp", source_os);
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
  return 0;
}