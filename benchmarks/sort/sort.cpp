#include "fheco/fheco.hpp"

using namespace std;
using namespace fheco;
#include <chrono>
#include <fstream>
#include <iostream> 
#include <string>
#include <vector>
#include <cmath>
#include "fheco/dsl/benchmark_types.cpp"
 
/**************************/
void fhe_vectorized(int width){
  throw invalid_argument("vectorized implementation doesnt exist");
}
/************************************/
Ciphertext cond(Ciphertext val ,Ciphertext  ch1,Ciphertext ch2){
    return val*ch1 + (1-val)*ch2;
}
void fhe(int width)
{ 
  Ciphertext c12("c12"); 
  Ciphertext c23("c23"); 
  Ciphertext c13("c13");
  Ciphertext o123("o123"); 
  Ciphertext o132("o132"); 
  Ciphertext o312("o312"); 
  Ciphertext o213("o213"); 
  Ciphertext o231("o231");
  Ciphertext o321("o321");   
  Ciphertext output ;
  output = cond(c12, (cond(c23,o123,cond(c13, o132, o312))), (cond(c13,o213,cond(c23, o231, o321))) );
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
  Compiler::disable_auto_enc_params_selection();
  chrono::high_resolution_clock::time_point t;
  chrono::duration<double, milli> elapsed;
  string func_name = "fhe";
  /**************/t = chrono::high_resolution_clock::now();
  if (vectorized)
  {
      int benchmark_type = STRUCTURED_WITH_MULTIPLE_OUTPUTS;
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
      //Compiler::gen_vectorized_code(func, window);
      //Compiler::gen_he_code(func, header_os, gen_name + ".hpp", source_os);
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
  else
  {
      const auto &func = Compiler::create_func(func_name, slot_count*slot_count, 20, false, true);
      //double squareRoot = sqrt(slot_count);
      //int width = static_cast<int>(squareRoot);
      fhe_vectorized(slot_count);
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
/*
 rw!("assoc-balan-add-mul-1"; 
        "(VecAdd (VecAdd (VecAdd (VecMul ?c1 ?c2) (VecMul ?d1 ?d2)) (VecMul ?b1 ?b2)) (VecMul ?a1 ?a2))" => 
        "(VecAdd (VecAdd (VecMul ?a1 ?a2) (VecMul ?b1 ?b2)) (VecAdd (VecMul ?c1 ?c2) (VecMul ?d1 ?d2)))"
        if is_vec_mul("?a1","?a2","?b1","?b2","?c1","?c2","?d1","?d2")
        ),
        rw!("assoc-balan-add-mul-2"; 
        "(VecAdd (VecAdd (VecMul ?b1 ?b2) (VecAdd (VecMul ?c1 ?c2) (VecMul ?d1 ?d2))) (VecMul ?a1 ?a2))" => 
        "(VecAdd (VecAdd (VecMul ?a1 ?a2) (VecMul ?b1 ?b2)) (VecAdd (VecMul ?c1 ?c2) (VecMul ?d1 ?d2)))"
        if is_vec_mul("?a1","?a2","?b1","?b2","?c1","?c2","?d1","?d2")
        ),
        rw!("assoc-balan-add-mul-3"; 
        "(VecAdd (VecAdd (VecMul ?a1 ?a2) (VecAdd (VecMul ?b1 ?b2) (VecMul ?c1 ?c2))) (VecMul ?d1 ?d2))" => 
        "(VecAdd (VecAdd (VecMul ?a1 ?a2) (VecMul ?b1 ?b2)) (VecAdd (VecMul ?c1 ?c2) (VecMul ?d1 ?d2)))"
        if is_vec_mul("?a1","?a2","?b1","?b2","?c1","?c2","?d1","?d2")
        ),
        rw!("distribute-mul-over-add"; 
        "(VecMul ?a (VecAdd ?b ?c))" => "(VecAdd (VecMul ?a ?b) (VecMul ?a ?c))"
        if is_vec("?a","?b","?c","?c")
        ),
*/