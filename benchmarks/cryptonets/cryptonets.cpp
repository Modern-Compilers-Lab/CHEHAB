#include "fheco/fheco.hpp"
#include <chrono>
#include <fstream>
#include <iostream> 
#include <string>
#include <vector>
#include <cstdint> 
#include <cstddef>
#include <stdexcept>
#include <memory> 
#include <algorithm>
using namespace std;
using namespace fheco;
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
/***********Support functions *******************/
vector<string> split(const string &str, char delim)
{
  vector<string> tokens;
  string token = "";
  for (const auto &c : str)
  {
    if (c == delim)
    {
      tokens.push_back(token);
      token = "";
    }
    else
      token += c;
  }
  tokens.push_back(token);
  return tokens;
}
/******************************************************************************************************************/
vector<vector<vector<vector<integer>>>> reshape_4d(const vector<vector<integer>> &data, const vector<size_t> &shape)
{
  if (shape.size() < 4)
    throw invalid_argument("incomplete dimensions");

  vector<vector<vector<vector<integer>>>> result(
    shape[0], vector<vector<vector<integer>>>(shape[1], vector<vector<integer>>(shape[2], vector<integer>(shape[3]))));
  size_t i, j, k, l;
  i = j = k = l = 0;
  bool full = false;
  for (const auto &vec : data)
  {
    for (const auto &e : vec)
    {
      result[i][j][k][l] = e;
      ++l;
      if (l == shape[3])
      {
        l = 0;
        ++k;
        if (k == shape[2])
        {
          k = 0;
          ++j;
          if (j == shape[1])
          {
            j = 0;
            ++i;
            if (i == shape[0])
            {
              full = true;
              break;
            }
          }
        }
      }
    }
    if (full)
      break;
  }
  return result;
}
/****************************************************/
vector<vector<integer>> load(istream &is, char delim)
{
  vector<vector<integer>> data;
  string line;
  while (getline(is, line))
  {
    auto tokens = split(line, delim);
    vector<integer> line_data;
    line_data.reserve(tokens.size());
    for (const auto &token : tokens)
      line_data.push_back(static_cast<integer>(stoull(token)));
    data.push_back(line_data);
  }
  return data;
}
/*********************************/
vector<integer> load(istream &is)
{
  vector<integer> data;
  string line;
  while (getline(is, line))
    data.push_back(static_cast<integer>(stoull(line)));
  return data;
}
/***************************************************/
vector<vector<vector<Ciphertext>>> pad_2d(
  const vector<vector<vector<Ciphertext>>> &input, const vector<size_t> &kernel_shape, const vector<size_t> &strides)
{
  auto n_rows_in = input.size();
  auto n_cols_in = input[0].size();
  auto n_channels_in = input[0][0].size();
  auto n_rows_kernel = kernel_shape[0];
  auto n_cols_kernel = kernel_shape[1];
  auto row_stride = strides[0];
  auto col_stride = strides[1];

  auto n_rows_out = (n_rows_in + 1) / row_stride;
  auto n_cols_out = (n_cols_in + 1) / col_stride;
  auto pad_rows = max((n_rows_out - 1) * row_stride + n_rows_kernel - n_rows_in, 0UL);
  auto pad_cols = max((n_cols_out - 1) * col_stride + n_cols_kernel - n_cols_in, 0UL);
  auto pad_top = pad_rows / 2;
  auto pad_bottom = pad_rows - pad_top;
  auto pad_left = pad_cols / 2;
  auto pad_right = pad_cols - pad_left;
  n_rows_out = n_rows_in + pad_rows;
  n_cols_out = n_cols_in + pad_cols;

  vector<vector<vector<Ciphertext>>> output(
    n_rows_out, vector<vector<Ciphertext>>(n_cols_out, vector<Ciphertext>(n_channels_in, encrypt(0))));
  for (size_t i = 0; i < n_rows_in; ++i)
  {
    for (size_t j = 0; j < n_cols_in; ++j)
      output[i + pad_top][j + pad_left] = input[i][j];
  }
  return output;
}
/**************************************************/
vector<vector<vector<Ciphertext>>> conv_2d(
  const vector<vector<vector<Ciphertext>>> &input, const vector<vector<vector<vector<Plaintext>>>> &kernels,
  const vector<size_t> &strides)
{
  auto n_channels_in = input[0][0].size();
  auto n_channels_kernel = kernels[0][0].size();
  if (n_channels_in != n_channels_kernel)
    throw invalid_argument("incompatible number of channels");

  auto n_rows_in = input.size();
  auto n_cols_in = input[0].size();
  auto n_rows_kernel = kernels.size();
  auto n_cols_kernel = kernels[0].size();
  auto row_stride = strides[0];
  auto col_stride = strides[1];

  auto padded_in = pad_2d(input, {n_rows_kernel, n_cols_kernel}, strides);
  auto n_rows_out = n_rows_in / row_stride + (n_rows_in % row_stride > 0 ? 1 : 0);
  auto n_cols_out = n_cols_in / col_stride + (n_cols_in % col_stride > 0 ? 1 : 0);
  auto n_channels_out = kernels[0][0][0].size();
  vector<vector<vector<Ciphertext>>> output(
    n_rows_out, vector<vector<Ciphertext>>(n_cols_out, vector<Ciphertext>(n_channels_out, encrypt(0))));
  size_t row_offset = 0;
  for (size_t i_out = 0; i_out < n_rows_out; ++i_out)
  {
    size_t col_offset = 0;
    for (size_t j_out = 0; j_out < n_cols_out; ++j_out)
    {
      for (size_t k_out = 0; k_out < n_channels_out; ++k_out)
      {
        for (size_t i_kernels = 0; i_kernels < n_rows_kernel; ++i_kernels)
          for (size_t j_kernels = 0; j_kernels < n_cols_kernel; ++j_kernels)
            for (size_t k_kernels = 0; k_kernels < n_channels_kernel; ++k_kernels)
              output[i_out][j_out][k_out] += padded_in[i_kernels + row_offset][j_kernels + col_offset][k_kernels] *
                                             kernels[i_kernels][j_kernels][k_kernels][k_out];
      }
      col_offset += col_stride;
    }
    row_offset += row_stride;
  }
  return output;
}
/*********************************************************************************/
vector<Ciphertext> add(const vector<Ciphertext> &input, const vector<Plaintext> &b)
{
  if (input.size() != b.size())
    throw invalid_argument("incompatible sizes");

  vector<Ciphertext> output(input.size());
  for (size_t i = 0; i < output.size(); ++i)
    output[i] = input[i] + b[i];

  return output;
}
/*****************************************************************************************************************/
vector<vector<vector<Ciphertext>>> add(const vector<vector<vector<Ciphertext>>> &input, const vector<Plaintext> &b)
{
  auto n_channels = input[0][0].size();
  if (n_channels != b.size())
    throw invalid_argument("incompatible sizes");

  auto n_rows = input.size();
  auto n_cols = input[0].size();
  vector<vector<vector<Ciphertext>>> output(n_rows, vector<vector<Ciphertext>>(n_cols, vector<Ciphertext>(n_channels)));
  for (size_t i = 0; i < n_rows; ++i)
    for (size_t j = 0; j < n_cols; ++j)
      for (size_t k = 0; k < n_channels; ++k)
        output[i][j][k] = input[i][j][k] + b[k];

  return output;
}
/*****************************************************************************************/
vector<Ciphertext> dot(const vector<Ciphertext> &input, const vector<vector<Plaintext>> &w)
{
  if (input.size() != w.size())
    throw invalid_argument("incompatible sizes");

  vector<Ciphertext> output(w[0].size(), encrypt(0));
  for (size_t i = 0; i < w[0].size(); ++i)
    for (size_t j = 0; j < w.size(); ++j)
      output[i] += input[j] * w[j][i];

  return output;
}
/**************************************************************************/
vector<Ciphertext> flatten(const vector<vector<vector<Ciphertext>>> &input)
{
  auto n_rows = input.size();
  auto n_cols = input[0].size();
  auto n_channels = input[0][0].size();
  vector<Ciphertext> output(n_rows * n_cols * n_channels);
  size_t i_out = 0;
  for (size_t i = 0; i < n_rows; ++i)
    for (size_t j = 0; j < n_cols; ++j)
      for (size_t k = 0; k < n_channels; ++k)
      {
        output[i_out] = input[i][j][k];
        ++i_out;
      }

  return output;
}
/****************************************************************************************/
vector<vector<vector<Ciphertext>>> square(const vector<vector<vector<Ciphertext>>> &input)
{
  auto n_rows = input.size();
  auto n_cols = input[0].size();
  auto n_channels = input[0][0].size();
  vector<vector<vector<Ciphertext>>> output(n_rows, vector<vector<Ciphertext>>(n_cols, vector<Ciphertext>(n_channels)));
  for (size_t i = 0; i < n_rows; ++i)
    for (size_t j = 0; j < n_cols; ++j)
      for (size_t k = 0; k < n_channels; ++k)
        output[i][j][k] = input[i][j][k] * input[i][j][k];

  return output;
}
/**********************************************************/
vector<Ciphertext> square(const vector<Ciphertext> &input)
{
  vector<Ciphertext> output(input.size());
  for (size_t i = 0; i < output.size(); ++i)
    output[i] = input[i] * input[i];

  return output;
}
/*********************************************************/
vector<vector<vector<Ciphertext>>> scaled_mean_pool_2d(
  const vector<vector<vector<Ciphertext>>> &input, const vector<size_t> &kernel_shape, const vector<size_t> &strides)
{
  auto n_rows_in = input.size();
  auto n_cols_in = input[0].size();
  auto n_channels_in = input[0][0].size();
  auto n_rows_kernel = kernel_shape[0];
  auto n_cols_kernel = kernel_shape[1];
  auto row_stride = strides[0];
  auto col_stride = strides[1];

  auto n_rows_out = n_rows_in / row_stride + (n_rows_in % row_stride > 0 ? 1 : 0);
  auto n_cols_out = n_cols_in / col_stride + (n_cols_in % col_stride > 0 ? 1 : 0);
  auto n_channels_output = input[0][0].size();
  vector<vector<vector<Ciphertext>>> output(
    n_rows_out, vector<vector<Ciphertext>>(n_cols_out, vector<Ciphertext>(n_channels_output, encrypt(0))));
  size_t row_offset = 0;
  for (size_t i_output = 0; i_output < n_rows_out; ++i_output)
  {
    size_t col_offset = 0;
    for (size_t j_output = 0; j_output < n_cols_out; ++j_output)
    {
      for (size_t k_output = 0; k_output < n_channels_output; ++k_output)
      {
        for (size_t i_kernel = 0; i_kernel < n_rows_kernel; ++i_kernel)
          for (size_t j_kernel = 0; j_kernel < n_cols_kernel; ++j_kernel)
            output[i_output][j_output][k_output] += input[i_kernel + row_offset][j_kernel + col_offset][k_output];
      }
      col_offset += col_stride;
    }
    row_offset += row_stride;
  }
  return output;
}
/***************************************************************************/
/***************************************************************************/
void fhe(int slot_count)
{
  vector<size_t> mean_pool_kernel_shape = {2, 2};
  vector<size_t> conv_strides = {2, 2};
  vector<size_t> image_shape = {28,28,1};
  vector<size_t> w1_shape = {5, 5, 1, 5};
  vector<size_t> w4_shape = {5, 5, 5, 10};
  vector<size_t> w8_shape = {40, 10};
  vector<size_t> b1_shape = {5};
  vector<size_t> b4_shape = {10};
  vector<size_t> b8_shape = {10};
  /***************************************************************************/
  /************** Params loading process ************************************/
  char delim = ' ';
  ifstream w1_is("w1.txt");
  if (!w1_is)
    throw invalid_argument("failed to open w1 file");
  vector<vector<vector<vector<integer>>>> w1_vals;
  {
    auto w1_raw = load(w1_is, delim); 
    w1_vals = reshape_4d(w1_raw, w1_shape);
  }
  vector<vector<vector<vector<Plaintext>>>> w1(
    w1_shape[0], vector<vector<vector<Plaintext>>>(
                   w1_shape[1], vector<vector<Plaintext>>(w1_shape[2], vector<Plaintext>(w1_shape[3]))));
  for (size_t i = 0; i < w1_shape[0]; ++i)
    for (size_t j = 0; j < w1_shape[1]; ++j) 
      for (size_t k = 0; k < w1_shape[2]; ++k)
        for (size_t l = 0; l < w1_shape[3]; ++l)
          w1[i][j][k][l] = w1_vals[i][j][k][l];
  /********************************************/
  ifstream w4_is("w4.txt");
  if (!w4_is)
    throw invalid_argument("failed to open w4 file");
  vector<vector<vector<vector<integer>>>> w4_vals;
  {
    auto w4_raw = load(w4_is, delim);
    w4_vals = reshape_4d(w4_raw, w4_shape);
  }
  vector<vector<vector<vector<Plaintext>>>> w4(w4_shape[0], vector<vector<vector<Plaintext>>>(
                   w4_shape[1], vector<vector<Plaintext>>(w4_shape[2], vector<Plaintext>(w4_shape[3]))));
  for (size_t i = 0; i < w4_shape[0]; ++i)
    for (size_t j = 0; j < w4_shape[1]; ++j)
      for (size_t k = 0; k < w4_shape[2]; ++k)
        for (size_t l = 0; l < w4_shape[3]; ++l)
          w4[i][j][k][l] = w4_vals[i][j][k][l];
  
  /************************************************/
  ifstream w8_is("w8.txt");
  if (!w8_is)
    throw invalid_argument("failed to open w8 file");
  vector<vector<integer>> w8_vals = load(w8_is, delim);
  vector<vector<Plaintext>> w8(w8_shape[0], vector<Plaintext>(w8_shape[1]));
  for (size_t i = 0; i < w8_shape[0]; ++i)
    for (size_t j = 0; j < w8_shape[1]; ++j)
      w8[i][j] = w8_vals[i][j];
  /***********************************************/
  /***********************************************/
  ifstream b1_is("b1.txt");
  if (!b1_is)
    throw invalid_argument("failed to open b1 file");
  auto b1_vals = load(b1_is);
  vector<Plaintext> b1(b1_shape[0]);
  for (size_t i = 0; i < b1_shape[0]; ++i)
    b1[i] = b1_vals[i];
  /************************************************/
  ifstream b4_is("b4.txt");
  if (!b4_is)
    throw invalid_argument("failed to open b4 file");
  auto b4_vals = load(b4_is);
  vector<Plaintext> b4(b4_shape[0]);
  for (size_t i = 0; i < b4_shape[0]; ++i)
    b4[i] = b4_vals[i];
  /************************************************/
  ifstream b8_is("b8.txt");
  if (!b8_is)
    throw invalid_argument("failed to open b8 file");
  auto b8_vals = load(b8_is);
  vector<Plaintext> b8(b8_shape[0]);
  for (size_t i = 0; i < b8_shape[0]; ++i)
    b8[i] = b8_vals[i];
  /**************************************************************/
  /**************************************************************/
  vector<vector<vector<Ciphertext>>> image(
    image_shape[0], vector<vector<Ciphertext>>(image_shape[1], vector<Ciphertext>(image_shape[2])));  
  // load image
  for (int i = 0; i < image_shape[0]; ++i)
  {
    for (int j = 0; j < image_shape[1]; ++j)
    {
      for (int k = 0; k < image_shape[2]; ++k)
      {
         image[i][j][k] = Ciphertext("in_" + std::to_string(i) + "_" + std::to_string(j)+"_"+ std::to_string(k));
      }
    }
  }
  
  // First operation CONV2D 
  auto h1 = add(conv_2d(image, w1, conv_strides), b1);
  auto h2 = square(h1);
  auto h3 = scaled_mean_pool_2d(h2, mean_pool_kernel_shape, conv_strides);
  auto h4 = add(conv_2d(h3, w4, conv_strides), b4);
  auto h5 = scaled_mean_pool_2d(h4, mean_pool_kernel_shape, mean_pool_kernel_shape);
  auto h6 = flatten(h5);
  auto h7 = square(h6);
  auto c = add(dot(h7, w8), b8);
  for (int i = 0; i < c.size(); ++i)
  {
    c[i].set_output("out_" + std::to_string(i));
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