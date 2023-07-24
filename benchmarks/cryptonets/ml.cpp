#include "ml.hpp"
#include <stdexcept>

using namespace std;
using namespace fheco;

vector<Ciphertext> predict(
  const vector<vector<vector<Ciphertext>>> &x, const vector<vector<vector<vector<Plaintext>>>> &w1,
  const vector<Plaintext> &b1, const vector<vector<vector<vector<Plaintext>>>> &w4, const vector<Plaintext> &b4,
  const vector<vector<Plaintext>> &w8, const vector<Plaintext> &b8)
{
  vector<size_t> conv_strides = {2, 2};
  vector<size_t> mean_pool_kernel_shape = {2, 2};

  auto h1 = add(conv_2d(x, w1, conv_strides), b1);
  auto h2 = square(h1);
  auto h3 = scaled_mean_pool_2d(h2, mean_pool_kernel_shape, mean_pool_kernel_shape);
  auto h4 = add(conv_2d(h3, w4, conv_strides), b4);
  auto h5 = scaled_mean_pool_2d(h4, mean_pool_kernel_shape, mean_pool_kernel_shape);
  auto h6 = flatten(h5);
  auto h7 = square(h6);
  auto h8 = add(dot(h7, w8), b8);
  return h8;
}

// HWC, zero padding, mapcount
vector<vector<vector<Ciphertext>>> conv_2d(
  const vector<vector<vector<Ciphertext>>> &input, const vector<vector<vector<vector<Plaintext>>>> &kernels,
  const vector<size_t> &strides)
{
  size_t n_channels_in = input[0][0].size();
  size_t n_channels_kernel = kernels[0][0].size();
  if (n_channels_in != n_channels_kernel)
    throw invalid_argument("incompatible number of channels");

  size_t n_rows_in = input.size();
  size_t n_cols_in = input[0].size();
  size_t n_rows_kernel = kernels.size();
  size_t n_cols_kernel = kernels[0].size();
  size_t row_stride = strides[0];
  size_t col_stride = strides[1];

  auto padded_in = pad_2d(input, {n_rows_kernel, n_cols_kernel}, strides);
  size_t n_rows_out = n_rows_in / row_stride + (n_rows_in % row_stride > 0 ? 1 : 0);
  size_t n_cols_out = n_cols_in / col_stride + (n_cols_in % col_stride > 0 ? 1 : 0);
  size_t n_channels_out = kernels[0][0][0].size();
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

// no padding, mapcount
vector<vector<vector<Ciphertext>>> scaled_mean_pool_2d(
  const vector<vector<vector<Ciphertext>>> &input, const vector<size_t> &kernel_shape, const vector<size_t> &strides)
{
  size_t n_rows_in = input.size();
  size_t n_cols_in = input[0].size();
  size_t n_channels_in = input[0][0].size();
  size_t n_rows_kernel = kernel_shape[0];
  size_t n_cols_kernel = kernel_shape[1];
  size_t row_stride = strides[0];
  size_t col_stride = strides[1];

  size_t n_rows_out = n_rows_in / row_stride + (n_rows_in % row_stride > 0 ? 1 : 0);
  size_t n_cols_out = n_cols_in / col_stride + (n_cols_in % col_stride > 0 ? 1 : 0);
  size_t n_channels_output = input[0][0].size();
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

vector<vector<vector<Ciphertext>>> pad_2d(
  const vector<vector<vector<Ciphertext>>> &input, const vector<size_t> &kernel_shape, const vector<size_t> &strides)
{
  size_t n_rows_in = input.size();
  size_t n_cols_in = input[0].size();
  size_t n_channels_in = input[0][0].size();
  size_t n_rows_kernel = kernel_shape[0];
  size_t n_cols_kernel = kernel_shape[1];
  size_t row_stride = strides[0];
  size_t col_stride = strides[1];

  size_t n_rows_out = (n_rows_in + 1) / row_stride;
  size_t n_cols_out = (n_cols_in + 1) / col_stride;
  size_t pad_rows = max((n_rows_out - 1) * row_stride + n_rows_kernel - n_rows_in, 0UL);
  size_t pad_cols = max((n_cols_out - 1) * col_stride + n_cols_kernel - n_cols_in, 0UL);
  size_t pad_top = pad_rows / 2;
  size_t pad_bottom = pad_rows - pad_top;
  size_t pad_left = pad_cols / 2;
  size_t pad_right = pad_cols - pad_left;
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

vector<Ciphertext> add(const vector<Ciphertext> &input, const vector<Plaintext> &b)
{
  if (input.size() != b.size())
    throw invalid_argument("incompatible sizes");

  vector<Ciphertext> output(input.size());
  for (size_t i = 0; i < output.size(); ++i)
    output[i] = input[i] + b[i];

  return output;
}

vector<vector<vector<Ciphertext>>> add(const vector<vector<vector<Ciphertext>>> &input, const vector<Plaintext> &b)
{
  size_t n_channels = input[0][0].size();
  if (n_channels != b.size())
    throw invalid_argument("incompatible sizes");

  size_t n_rows = input.size();
  size_t n_cols = input[0].size();
  vector<vector<vector<Ciphertext>>> output(n_rows, vector<vector<Ciphertext>>(n_cols, vector<Ciphertext>(n_channels)));
  for (size_t i = 0; i < n_rows; ++i)
    for (size_t j = 0; j < n_cols; ++j)
      for (size_t k = 0; k < n_channels; ++k)
        output[i][j][k] = input[i][j][k] + b[k];

  return output;
}

vector<vector<vector<Ciphertext>>> square(const vector<vector<vector<Ciphertext>>> &input)
{
  size_t n_rows = input.size();
  size_t n_cols = input[0].size();
  size_t n_channels = input[0][0].size();
  vector<vector<vector<Ciphertext>>> output(n_rows, vector<vector<Ciphertext>>(n_cols, vector<Ciphertext>(n_channels)));
  for (size_t i = 0; i < n_rows; ++i)
    for (size_t j = 0; j < n_cols; ++j)
      for (size_t k = 0; k < n_channels; ++k)
        output[i][j][k] = input[i][j][k] * input[i][j][k];

  return output;
}

vector<Ciphertext> square(const vector<Ciphertext> &input)
{
  vector<Ciphertext> output(input.size());
  for (size_t i = 0; i < output.size(); ++i)
    output[i] = input[i] * input[i];

  return output;
}

// mapcount
// w for weights
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

vector<Ciphertext> flatten(const vector<vector<vector<Ciphertext>>> &input)
{
  size_t n_rows = input.size();
  size_t n_cols = input[0].size();
  size_t n_channels = input[0][0].size();
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
