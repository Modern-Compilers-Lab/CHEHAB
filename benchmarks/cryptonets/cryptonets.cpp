#include "fhecompiler/fhecompiler.hpp"
#include "ml.hpp"
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
  vector<vector<vector<Ciphertext>>> x;

  for (size_t i = 0; i < x_shape[0]; ++i)
  {
    x.push_back(vector<vector<Ciphertext>>());
    for (size_t j = 0; j < x_shape[1]; ++j)
    {
      x[i].push_back(vector<Ciphertext>());
      for (size_t k = 0; k < x_shape[2]; ++k)
      {
        Ciphertext xx("x[" + to_string(i) + "][" + to_string(j) + "][" + to_string(k) + "]", VarType::input);
        x[i][j].push_back(xx);
      }
    }
  }
  vector<vector<vector<vector<Plaintext>>>> w1;
  for (size_t i = 0; i < w1_shape[0]; ++i)
  {
    w1.push_back(vector<vector<vector<Plaintext>>>());
    for (size_t j = 0; j < w1_shape[1]; ++j)
    {
      w1[i].push_back(vector<vector<Plaintext>>());
      for (size_t k = 0; k < w1_shape[2]; ++k)
      {
        w1[i][j].push_back(vector<Plaintext>());
        for (size_t l = 0; l < w1_shape[3]; ++l)
        {
          Plaintext pt(
            "w1[" + to_string(i) + "][" + to_string(j) + "][" + to_string(k) + "][" + to_string(l) + "]",
            VarType::input);
          w1[i][j][k].push_back(pt);
        }
      }
    }
  }

  vector<Plaintext> b1;
  for (size_t i = 0; i < b1_shape[0]; ++i)
  {
    Plaintext pt("b1[" + to_string(i) + "]", VarType::input);
    b1.push_back(pt);
  }

  vector<vector<vector<vector<Plaintext>>>> w4;
  for (size_t i = 0; i < w4_shape[0]; ++i)
  {
    w4.push_back(vector<vector<vector<Plaintext>>>());
    for (size_t j = 0; j < w4_shape[1]; ++j)
    {
      w4[i].push_back(vector<vector<Plaintext>>());
      for (size_t k = 0; k < w4_shape[2]; ++k)
      {
        w4[i][j].push_back(vector<Plaintext>());
        for (size_t l = 0; l < w4_shape[3]; ++l)
        {
          Plaintext pt(
            "w4[" + to_string(i) + "][" + to_string(j) + "][" + to_string(k) + "][" + to_string(l) + "]",
            VarType::input);
        }
      }
    }
  }

  vector<Plaintext> b4;
  for (size_t i = 0; i < b4_shape[0]; ++i)
  {
    Plaintext pt("b4[" + to_string(i) + "]", VarType::input);
    b4.push_back(pt);
  }

  vector<vector<Plaintext>> w8;
  for (size_t i = 0; i < w8_shape[0]; ++i)
  {
    w8.push_back(vector<Plaintext>());
    for (size_t j = 0; j < w8_shape[1]; ++j)
    {
      Plaintext pt("w8[" + to_string(i) + "][" + to_string(j) + "]", VarType::input);
      w8[i].push_back(pt);
    }
  }

  vector<Plaintext> b8;
  for (size_t i = 0; i < b8_shape[0]; ++i)
  {
    Plaintext pt("b8[" + to_string(i) + "]", VarType::input);
    b8.push_back(pt);
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
