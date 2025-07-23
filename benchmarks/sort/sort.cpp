#include "fheco/fheco.hpp"

using namespace std;
using namespace fheco;
#include <chrono>
#include <fstream>
#include <iostream>   
#include <string>
#include <vector>
#include <cmath>
#include "../global_variables.hpp"  
/**************************/
void fhe_vectorized(int width){
  throw invalid_argument("vectorized implementation doesnt exist");
}
/************************************/
Ciphertext cond(Ciphertext val ,Ciphertext  ch1,Ciphertext ch2){
    return val*ch1 + (1-val)*ch2;
}
void fhe(int slot_count) 
{
  Ciphertext output ;
  Ciphertext c12("c12"); 
  Ciphertext c23("c23"); 
  Ciphertext c13("c13");
  Ciphertext c24("c24"); 
  Ciphertext c14("c14");  
  Ciphertext c34("c34"); 
  // sort 3 
  Ciphertext o123("o123"); 
  Ciphertext o132("o132"); 
  Ciphertext o213("o213");  
  Ciphertext o231("o231");
  Ciphertext o321("o321");  
  Ciphertext o312("o312"); 
  // sort 4 
  Ciphertext o1234("o1234");  
  Ciphertext o1243("o1243");  
  Ciphertext o1324("o1324");  
  Ciphertext o1342("o1342");  
  Ciphertext o1423("o1423");  
  Ciphertext o1432("o1432");  
  Ciphertext o2134("o2134");  
  Ciphertext o2143("o2143");  
  Ciphertext o2314("o2314");  
  Ciphertext o2341("o2341");  
  Ciphertext o2413("o2413");  
  Ciphertext o2431("o2431");  
  Ciphertext o3124("o3124");  
  Ciphertext o3142("o3142");  
  Ciphertext o3214("o3214");  
  Ciphertext o3241("o3241");  
  Ciphertext o3412("o3412");  
  Ciphertext o3421("o3421");  
  Ciphertext o4123("o4123");  
  Ciphertext o4132("o4132");  
  Ciphertext o4213("o4213");  
  Ciphertext o4231("o4231");  
  Ciphertext o4312("o4312");  
  Ciphertext o4321("o4321");

  if(slot_count == 3){
    output = cond(c12, (cond(c23,o123,cond(c13, o132, o312))), (cond(c13,o213,cond(c23, o231, o321))) );
  } else if (slot_count == 4){
    output =  cond(c12,
        // Case 1: v1 ≤ v2
        cond(c13,
          // Subcase 1.1: v1 ≤ v2 and v1 ≤ v3 → v1 is smallest
          cond(c23,
            // Subcase 1.1.1: v1 ≤ v2 ≤ v3 → now insert v4
            cond(c14,
              cond(c24,
                cond(c34, o1234, o1243),  // v1 ≤ v2 ≤ v3 ≤ v4 or v1 ≤ v2 ≤ v4 ≤ v3
                cond(c14, o1423, o4123)   // v1 ≤ v4 ≤ v2 ≤ v3 or v4 ≤ v1 ≤ v2 ≤ v3
              ),
              o1234  // Fallback (shouldn't occur if comparisons are consistent)
            ),
            // Subcase 1.1.2: v1 ≤ v3 ≤ v2 → insert v4
            cond(c14,
              cond(c34,
                cond(c24, o1324, o1342),  // v1 ≤ v3 ≤ v2 ≤ v4 or v1 ≤ v3 ≤ v4 ≤ v2
                cond(c14, o1432, o4132)    // v1 ≤ v4 ≤ v3 ≤ v2 or v4 ≤ v1 ≤ v3 ≤ v2
              ),
              o1324  // Fallback
            )
          ),
          // Subcase 1.2: v1 ≤ v2 and v3 ≤ v1 → v3 ≤ v1 ≤ v2 → insert v4
          cond(c34,
            cond(c14,
              cond(c24, o3124, o3142),  // v3 ≤ v1 ≤ v2 ≤ v4 or v3 ≤ v1 ≤ v4 ≤ v2
              cond(c34, o3412, o4312)    // v3 ≤ v4 ≤ v1 ≤ v2 or v4 ≤ v3 ≤ v1 ≤ v2
            ),
            o3124  // Fallback
          )
        ),
        // Case 2: v2 ≤ v1
        cond(c23,
          // Subcase 2.1: v2 ≤ v1 and v2 ≤ v3 → v2 is smallest
          cond(c13,
            // Subcase 2.1.1: v2 ≤ v1 ≤ v3 → insert v4
            cond(c24,
              cond(c14,
                cond(c34, o2134, o2143),  // v2 ≤ v1 ≤ v3 ≤ v4 or v2 ≤ v1 ≤ v4 ≤ v3
                cond(c24, o2413, o4213)    // v2 ≤ v4 ≤ v1 ≤ v3 or v4 ≤ v2 ≤ v1 ≤ v3
              ),
              o2134  // Fallback
            ),
            // Subcase 2.1.2: v2 ≤ v3 ≤ v1 → insert v4
            cond(c24,
              cond(c34,
                cond(c14, o2314, o2341),  // v2 ≤ v3 ≤ v1 ≤ v4 or v2 ≤ v3 ≤ v4 ≤ v1
                cond(c24, o2431, o4231)    // v2 ≤ v4 ≤ v3 ≤ v1 or v4 ≤ v2 ≤ v3 ≤ v1
              ), 
              o2314  // Fallback
            )
          ),
          // Subcase 2.2: v2 ≤ v1 and v3 ≤ v2 → v3 ≤ v2 ≤ v1 → insert v4
          cond(c34,
            cond(c24,
              cond(c14, o3214, o3241),  // v3 ≤ v2 ≤ v1 ≤ v4 or v3 ≤ v2 ≤ v4 ≤ v1
              cond(c34, o3421, o4321)    // v3 ≤ v4 ≤ v2 ≤ v1 or v4 ≤ v3 ≤ v2 ≤ v1
            ),
            o3214  // Fallback
          )
        )
      );
  }
  output.set_output("output");
}
/******************************************************************************************/
/******************************************************************************************/
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
    Compiler::compile(func, ruleset, rewrite_heuristic, header_os, gen_name + ".hpp", source_os);
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
    const auto &func = Compiler::create_func(func_name, 1, 20, false, true);
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
