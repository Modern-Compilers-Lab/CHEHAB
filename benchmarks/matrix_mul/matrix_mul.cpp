#include "fheco/fheco.hpp"

using namespace std;
using namespace fheco;
#include <chrono>
#include <fstream>
#include <iostream> 
#include <string>
#include <vector> 
/************/
void fhe_vectorized(int slot_count){
  int m_a = slot_count ;
  int n_b = slot_count ;
  vector<Ciphertext> A_row_encrypted ;
  vector<Ciphertext> B_column_encrypted ;
  for (int i = 0; i < m_a; ++i)
  {
    Ciphertext line("A[" + to_string(i) + "]");
    A_row_encrypted.push_back(line);
  }
  for (int i = 0; i < n_b ; ++i)
  {
    Ciphertext column("B[" + to_string(i) + "]");
    B_column_encrypted.push_back(column);
  }
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            
  vector<Ciphertext> C_row_encrypted;
  for (size_t i = 0; i < A_row_encrypted.size(); ++i)
  {
    Ciphertext cline;
    for (size_t j = 0; j < B_column_encrypted.size(); ++j)
    {
      vector<int64_t> mask(A_row_encrypted.size(), 0);
      mask[j] = 1;
      Ciphertext slot; 
      slot = SumVec(A_row_encrypted[i] * B_column_encrypted[j],n_b);
      if (j == 0)                                                               
        cline = slot * mask;
      else
        cline += slot * mask;
      //slot.set_output("C[" + to_string(i) + "]["+ to_string(j) +"]");
    } 
    cline.set_output("C[" + to_string(i) + "]");
    C_row_encrypted.push_back(cline);
  }  
}
/***********************************************/
void fhe(int slot_count)
{
  size_t size = slot_count;
  std::vector<std::vector<Ciphertext>> a = std::vector<std::vector<Ciphertext>>(size, std::vector<Ciphertext>(size));
  std::vector<std::vector<Ciphertext>> b = std::vector<std::vector<Ciphertext>>(size, std::vector<Ciphertext>(size));
  std::vector<std::vector<Ciphertext>> c(size, std::vector<Ciphertext>(size));
  for (int i = 0; i < size; ++i)
  {
    for (int j = 0; j < size; ++j)
    {
      a[i][j] = Ciphertext("a_" + std::to_string(i) + "_" + std::to_string(j));
    }
  }
  for (int i = 0; i < size; ++i)
  {
    for (int j = 0; j < size; ++j)
    {
      b[i][j] = Ciphertext("b_" + std::to_string(i) + "_" + std::to_string(j));
    }
  }
  for (int i = 0; i < size; ++i)
  {
    for (int j = 0; j < size; ++j)
    {
      c[i][j] = a[i][0] * b[0][j];
      for (size_t k = 1; k < size; k++)
        c[i][j] += a[i][k] * b[k][j];
      c[i][j].set_output("c_" + std::to_string(i) + "_" + std::to_string(j));
    }
  } 
}
/************************************************************/
void print_bool_arg(bool arg, const string &name, ostream &os)
{
  os << (arg ? name : "no_" + name);
}
/************************************************************/
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
      Compiler::gen_vectorized_code(func);
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