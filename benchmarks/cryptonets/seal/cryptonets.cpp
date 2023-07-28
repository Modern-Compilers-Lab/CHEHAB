#include "ml.hpp"
#include "seal/seal.hpp"
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <ostream>
#include <string>
#include <vector>

using namespace std;
using namespace fhecompiler;

void cryptonets(
  const vector<size_t> &x_shape, const vector<size_t> &w1_shape, const vector<size_t> &b1_shape,
  const vector<size_t> &w4_shape, const vector<size_t> &b4_shape, const vector<size_t> &w8_shape,
  const vector<size_t> &b8_shape)
{
  // declare inputs
  vector<vector<vector<Ciphertext>>> x(
    x_shape[0], vector<vector<Ciphertext>>(x_shape[1], vector<Ciphertext>(x_shape[2])));

  for (int64_t i = 0; i < x_shape[0]; ++i)
  {
    for (int64_t j = 0; j < x_shape[1]; ++j)
    {
      for (int64_t k = 0; k < x_shape[2]; ++k)
      {

        // Ciphertext xx("x[" + to_string(i) + "][" + to_string(j) + "][" + to_string(k) + "]", VarType::input);
        Ciphertext xx = Ciphertext::encrypt(std::vector<int64_t>({i + j + k + 1}));
        x[i][j][k] = xx;
      }
    }
  }

  vector<vector<vector<vector<Plaintext>>>> w1(
    w1_shape[0], vector<vector<vector<Plaintext>>>(
                   w1_shape[1], vector<vector<Plaintext>>(w1_shape[2], vector<Plaintext>(w1_shape[3]))));
  for (int64_t i = 0; i < w1_shape[0]; ++i)
  {
    for (int64_t j = 0; j < w1_shape[1]; ++j)
    {
      for (int64_t k = 0; k < w1_shape[2]; ++k)
      {
        for (int64_t l = 0; l < w1_shape[3]; ++l)
        {
          /* Plaintext pt(
            "w1[" + to_string(i) + "][" + to_string(j) + "][" + to_string(k) + "][" + to_string(l) + "]",
            VarType::input); */
          Plaintext pt(std::vector<int64_t>({i + j + k + l + 1}));
          w1[i][j][k][l] = pt;
        }
      }
    }
  }

  vector<Plaintext> b1(b1_shape[0]);
  for (int64_t i = 0; i < b1_shape[0]; ++i)
  {
    // Plaintext pt("b1[" + to_string(i) + "]", VarType::input);
    Plaintext pt(std::vector<int64_t>({i}));
    b1[i] = pt;
  }

  vector<vector<vector<vector<Plaintext>>>> w4(
    w4_shape[0], vector<vector<vector<Plaintext>>>(
                   w4_shape[1], vector<vector<Plaintext>>(w4_shape[2], vector<Plaintext>(w4_shape[3]))));
  for (int64_t i = 0; i < w4_shape[0]; ++i)
  {
    for (int64_t j = 0; j < w4_shape[1]; ++j)
    {
      for (int64_t k = 0; k < w4_shape[2]; ++k)
      {
        for (int64_t l = 0; l < w4_shape[3]; ++l)
        {
          /*
          Plaintext pt(
            "w4[" + to_string(i) + "][" + to_string(j) + "][" + to_string(k) + "][" + to_string(l) + "]",
            VarType::input);
          */
          Plaintext pt(std::vector<int64_t>({i + j + k + l + 1}));
          w4[i][j][k][l] = pt;
        }
      }
    }
  }

  vector<Plaintext> b4(b4_shape[0]);
  for (int64_t i = 0; i < b4_shape[0]; ++i)
  {
    // Plaintext pt("b4[" + to_string(i) + "]", VarType::input);
    Plaintext pt(std::vector<int64_t>({i}));
    b4[i] = pt;
  }

  vector<vector<Plaintext>> w8(w8_shape[0], vector<Plaintext>(w8_shape[1]));
  for (int64_t i = 0; i < w8_shape[0]; ++i)
  {
    for (int64_t j = 0; j < w8_shape[1]; ++j)
    {
      // Plaintext pt("w8[" + to_string(i) + "][" + to_string(j) + "]", VarType::input);
      Plaintext pt(std::vector<int64_t>({i + j + 1}));
      w8[i][j] = pt;
    }
  }

  vector<Plaintext> b8(b8_shape[0]);
  for (int64_t i = 0; i < b8_shape[0]; ++i)
  {
    // Plaintext pt("b8[" + to_string(i) + "]", VarType::input);
    Plaintext pt(std::vector<int64_t>({i}));
    b8[i] = pt;
  }
  std::cout << "prediction time...\n";
  // predict
  auto y = predict(x, w1, b1, w4, b4, w8, b8);
  // declare outputs
  std::cout << "predict done.\n";
  for (size_t i = 0; i < y.size(); ++i)
  {
    // y[i].set_output("y[" + to_string(i) + "]");
    Ciphertext y_output("y_" + std::to_string(i), VarType::output);
    y_output = y[i];
  }
}

void print_bool_arg(bool arg, const string &name, ostream &os)
{
  os << (arg ? name : "no_" + name);
}

int main(int argc, char **argv)
{

  fhecompiler::init("cryptonets", fhecompiler::Scheme::bfv, fhecompiler::Backend::SEAL);

  size_t polynomial_modulus_degree = 4096;
  size_t plaintext_modulus = 786433;

  cryptonets(
    vector<size_t>{28, 28, 1}, vector<size_t>{5, 5, 1, 5}, vector<size_t>{5}, vector<size_t>{5, 5, 5, 10},
    vector<size_t>{10}, vector<size_t>{40, 10}, vector<size_t>{10});

  params_selector::EncryptionParameters params;
  params.set_plaintext_modulus(plaintext_modulus);
  params.set_polynomial_modulus_degree(polynomial_modulus_degree);

  compile("matrix_mul_poc_log2.hpp", &params);

  return 0;
}
