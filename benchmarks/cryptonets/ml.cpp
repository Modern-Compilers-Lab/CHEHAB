#include "ml.hpp"
#include <stdexcept>

using namespace std;
using namespace fhecompiler;

vector<Ciphertext> predict(
  const vector<vector<vector<Ciphertext>>> &x, const vector<vector<vector<vector<Plaintext>>>> &w1,
  const vector<Plaintext> &b1, const vector<vector<vector<vector<Plaintext>>>> &w4, const vector<Plaintext> &b4,
  const vector<vector<Plaintext>> &w8, const vector<Plaintext> &b8)
{
  vector<size_t> conv_strides = {2, 2};
  vector<size_t> mean_pool_kernel_size = {2, 2};

  chrono::high_resolution_clock::time_point t = chrono::high_resolution_clock::now();
  auto h1 = add(conv_2d(x, w1, conv_strides), b1);
  show_info("convadd", t, h1, "h1");

  t = chrono::high_resolution_clock::now();
  auto h2 = square(h1);
  show_info("square", t, h2, "h2");

  t = chrono::high_resolution_clock::now();
  auto h3 = scaled_mean_pool_2d(h2, mean_pool_kernel_size, mean_pool_kernel_size);
  show_info("meanpool", t, h3, "h3");

  t = chrono::high_resolution_clock::now();
  auto h4 = add(conv_2d(h3, w4, conv_strides), b4);
  show_info("convadd", t, h4, "h4");

  t = chrono::high_resolution_clock::now();
  auto h5 = scaled_mean_pool_2d(h4, mean_pool_kernel_size, mean_pool_kernel_size);
  show_info("meanpool", t, h5, "h5");

  t = chrono::high_resolution_clock::now();
  auto h6 = flatten(h5);
  show_info("reshape", t, h6, "h6");

  t = chrono::high_resolution_clock::now();
  auto h7 = square(h6);
  show_info("square  ", t, h7, "h7");

  t = chrono::high_resolution_clock::now();
  auto h8 = add(dot(h7, w8), b8);
  show_info("dotadd  ", t, h8, "h8");
  return h8;
}

// HWC, zero padding, mapcount
vector<vector<vector<Ciphertext>>> conv_2d(
  const vector<vector<vector<Ciphertext>>> &input, const vector<vector<vector<vector<Plaintext>>>> &filters,
  const vector<size_t> &strides)
{
  size_t n_channels_input = input[0][0].size();
  size_t n_channels_kernel = filters[0][0].size();
  if (n_channels_input != n_channels_kernel)
    throw invalid_argument("incompatible number of channels");

  size_t n_channels = n_channels_kernel;

  auto padded_input = pad_2d(input, vector<size_t>{filters.size(), filters[0].size()}, strides);
  size_t n_rows_output = input.size() / strides[0] + (input.size() % strides[0] > 0 ? 1 : 0);
  size_t n_cols_output = input[0].size() / strides[1] + (input[0].size() % strides[1] > 0 ? 1 : 0);
  size_t n_channels_output = filters[0][0][0].size();
  vector<vector<vector<Ciphertext>>> output(
    n_rows_output, vector<vector<Ciphertext>>(n_cols_output, vector<Ciphertext>(n_channels_output, Scalar(0))));
  size_t row_offset = 0;
  for (size_t i_output = 0; i_output < n_rows_output; ++i_output)
  {
    size_t col_offset = 0;
    for (size_t j_output = 0; j_output < n_cols_output; ++j_output)
    {
      for (size_t k_output = 0; k_output < n_channels_output; ++k_output)
      {
        for (size_t i_filters = 0; i_filters < filters.size(); ++i_filters)
          for (size_t j_filters = 0; j_filters < filters[0].size(); ++j_filters)
            for (size_t k = 0; k < n_channels; ++k)
              output[i_output][j_output][k_output] += padded_input[row_offset + i_filters][col_offset + j_filters][k] *
                                                      filters[i_filters][j_filters][k][k_output];
      }
      col_offset += strides[1];
    }
    row_offset += strides[0];
  }
  return output;
}

// no padding, mapcount
vector<vector<vector<Ciphertext>>> scaled_mean_pool_2d(
  const vector<vector<vector<Ciphertext>>> &input, const vector<size_t> &kernel_size, const vector<size_t> &strides)
{
  size_t n_rows_output = input.size() / strides[0] + (input.size() % strides[0] > 0 ? 1 : 0);
  size_t n_cols_output = input[0].size() / strides[1] + (input[0].size() % strides[1] > 0 ? 1 : 0);
  size_t n_channels_output = input[0][0].size();
  vector<vector<vector<Ciphertext>>> output(
    n_rows_output, vector<vector<Ciphertext>>(n_cols_output, vector<Ciphertext>(n_channels_output, Scalar(0))));
  size_t row_offset = 0;
  for (size_t i_output = 0; i_output < n_rows_output; ++i_output)
  {
    size_t col_offset = 0;
    for (size_t j_output = 0; j_output < n_cols_output; ++j_output)
    {
      for (size_t k_output = 0; k_output < n_channels_output; ++k_output)
      {
        for (size_t i_kernel = 0; i_kernel < kernel_size[0]; ++i_kernel)
          for (size_t j_kernel = 0; j_kernel < kernel_size[1]; ++j_kernel)
            output[i_output][j_output][k_output] += input[row_offset + i_kernel][col_offset + j_kernel][k_output];
      }
      col_offset += strides[1];
    }
    row_offset += strides[0];
  }
  return output;
}

vector<vector<vector<Ciphertext>>> pad_2d(
  const vector<vector<vector<Ciphertext>>> &input, const vector<size_t> &kernel_size, const vector<size_t> &strides)
{
  int n_rows_input = input.size();
  int n_cols_input = input[0].size();
  int n_channels = input[0][0].size();
  int n_rows_kernel = kernel_size[0];
  int n_cols_kernel = kernel_size[1];
  int row_stride = strides[0];
  int cols_stride = strides[1];

  // rows padding
  int pad_top = (n_rows_kernel - 1) / 2;
  int pad_bottom = max(
    n_rows_kernel - pad_top - 1 - (n_rows_input % row_stride == 0 ? row_stride - 1 : n_rows_input % row_stride - 1), 0);
  int pad_rows = pad_top + pad_bottom;
  // col padding
  int pad_left = (n_cols_kernel - 1) / 2;
  int pad_right = max(
    n_cols_kernel - pad_left - 1 - (n_cols_input % cols_stride == 0 ? cols_stride - 1 : n_cols_input % cols_stride - 1),
    0);
  int pad_cols = pad_left + pad_right;

  vector<vector<vector<Ciphertext>>> output(
    n_rows_input + pad_rows,
    vector<vector<Ciphertext>>(n_cols_input + pad_cols, vector<Ciphertext>(n_channels, Scalar(0))));
  for (int i = 0; i < n_rows_input; ++i)
  {
    for (size_t j = 0; j < n_cols_input; ++j)
      output[i + pad_top][j + pad_left] = input[i][j];
  }
  return output;
}

vector<vector<vector<Ciphertext>>> add(const vector<vector<vector<Ciphertext>>> &input, const vector<Plaintext> &b)
{
  if (input[0][0].size() != b.size())
    throw invalid_argument("incompatible sizes");

  auto output = input;
  for (size_t i = 0; i < output.size(); ++i)
    for (size_t j = 0; j < output[0].size(); ++j)
      for (size_t k = 0; k < output[0][0].size(); ++k)
        output[i][j][k] += b[k];

  return output;
}

vector<Ciphertext> add(const vector<Ciphertext> &input, const vector<Plaintext> &b)
{
  if (input.size() != b.size())
    throw invalid_argument("incompatible sizes");

  auto output = input;
  for (size_t i = 0; i < output.size(); ++i)
    output[i] += b[i];

  return output;
}

vector<vector<vector<Ciphertext>>> square(const vector<vector<vector<Ciphertext>>> &input)
{
  auto output = input;
  for (size_t i = 0; i < output.size(); ++i)
    for (size_t j = 0; j < output[0].size(); ++j)
      for (size_t k = 0; k < output[0][0].size(); ++k)
        output[i][j][k] *= output[i][j][k];

  return output;
}

vector<Ciphertext> square(const vector<Ciphertext> &input)
{
  auto output = input;
  for (size_t i = 0; i < output.size(); ++i)
    output[i] *= output[i];

  return output;
}

// mapcount
vector<Ciphertext> dot(const vector<Ciphertext> &input, const vector<vector<Plaintext>> &w)
{
  if (input.size() != w.size())
    throw invalid_argument("incompatible sizes");

  vector<Ciphertext> output(w[0].size(), Scalar(0));
  for (size_t i = 0; i < w[0].size(); ++i)
    for (size_t j = 0; j < input.size(); ++j)
      output[i] += input[j] * w[j][i];

  return output;
}

vector<Ciphertext> flatten(const vector<vector<vector<Ciphertext>>> &input)
{
  vector<Ciphertext> output(input.size() * input[0].size() * input[0][0].size(), Ciphertext());
  size_t i_output = 0;
  for (size_t i = 0; i < input.size(); ++i)
    for (size_t j = 0; j < input[0].size(); ++j)
      for (size_t k = 0; k < input[0][0].size(); ++k)
      {
        output[i_output] = input[i][j][k];
        ++i_output;
      }

  return output;
}
