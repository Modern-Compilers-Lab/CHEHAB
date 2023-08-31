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

void cryptonets()
{
  vector<size_t> x_shape = {28, 28, 1};
  vector<size_t> w1_shape = {5, 5, 1, 5};
  vector<size_t> b1_shape = {5};
  vector<size_t> w4_shape = {5, 5, 5, 10};
  vector<size_t> b4_shape = {10};
  vector<size_t> w8_shape = {40, 10};
  vector<size_t> b8_shape = {10};
  // load constants
  int64_t modulus = 65537;
  string constants_loc = "./constants/plain_mod_" + to_string(modulus) + "/";
  ifstream w1_is(constants_loc + "w1.txt");
  if (!w1_is)
    throw invalid_argument("failed to open w1 file");

  ifstream w4_is(constants_loc + "w4.txt");
  if (!w4_is)
    throw invalid_argument("failed to open w4 file");

  ifstream w8_is(constants_loc + "w8.txt");
  if (!w8_is)
    throw invalid_argument("failed to open w8 file");

  ifstream b1_is(constants_loc + "b1.txt");
  if (!b1_is)
    throw invalid_argument("failed to open b1 file");

  ifstream b4_is(constants_loc + "b4.txt");
  if (!b4_is)
    throw invalid_argument("failed to open b4 file");

  ifstream b8_is(constants_loc + "b8.txt");
  if (!b8_is)
    throw invalid_argument("failed to open b8 file");

  char delim = ' ';
  vector<vector<vector<vector<integer>>>> w1_vals;
  {
    auto w1_raw = load(w1_is, delim);
    w1_vals = reshape_4d(w1_raw, w1_shape);
  }
  vector<vector<vector<vector<integer>>>> w4_vals;
  {
    auto w4_raw = load(w4_is, delim);
    w4_vals = reshape_4d(w4_raw, w4_shape);
  }
  auto w8_vals = load(w8_is, delim);
  auto b1_vals = load(b1_is);
  auto b4_vals = load(b4_is);
  auto b8_vals = load(b8_is);

  // declare constants
  vector<vector<vector<vector<Scalar>>>> w1(
    w1_shape[0],
    vector<vector<vector<Scalar>>>(w1_shape[1], vector<vector<Scalar>>(w1_shape[2], vector<Scalar>(w1_shape[3]))));
  for (size_t i = 0; i < w1_shape[0]; ++i)
    for (size_t j = 0; j < w1_shape[1]; ++j)
      for (size_t k = 0; k < w1_shape[2]; ++k)
        for (size_t l = 0; l < w1_shape[3]; ++l)
          w1[i][j][k][l] = w1_vals[i][j][k][l];

  vector<vector<vector<vector<Scalar>>>> w4(
    w4_shape[0],
    vector<vector<vector<Scalar>>>(w4_shape[1], vector<vector<Scalar>>(w4_shape[2], vector<Scalar>(w4_shape[3]))));
  for (size_t i = 0; i < w4_shape[0]; ++i)
    for (size_t j = 0; j < w4_shape[1]; ++j)
      for (size_t k = 0; k < w4_shape[2]; ++k)
        for (size_t l = 0; l < w4_shape[3]; ++l)
        {
          w4[i][j][k][l] = w4_vals[i][j][k][l];
        }

  vector<vector<Scalar>> w8(w8_shape[0], vector<Scalar>(w8_shape[1]));
  for (size_t i = 0; i < w8_shape[0]; ++i)
    for (size_t j = 0; j < w8_shape[1]; ++j)
      w8[i][j] = w8_vals[i][j];

  vector<Scalar> b1(b1_shape[0]);
  for (size_t i = 0; i < b1_shape[0]; ++i)
    b1[i] = b1_vals[i];

  vector<Scalar> b4(b4_shape[0]);
  for (size_t i = 0; i < b4_shape[0]; ++i)
    b4[i] = b4_vals[i];

  vector<Scalar> b8(b8_shape[0]);
  for (size_t i = 0; i < b8_shape[0]; ++i)
    b8[i] = b8_vals[i];

  // declare inputs
  vector<vector<vector<Ciphertext>>> x(
    x_shape[0], vector<vector<Ciphertext>>(x_shape[1], vector<Ciphertext>(x_shape[2])));
  for (size_t i = 0; i < x_shape[0]; ++i)
    for (size_t j = 0; j < x_shape[1]; ++j)
      for (size_t k = 0; k < x_shape[2]; ++k)
      {
        Ciphertext input("x[" + to_string(i) + "][" + to_string(j) + "][" + to_string(k) + "]", VarType::input);
        x[i][j][k] = input;
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

  cryptonets();

  params_selector::EncryptionParameters params;
  params.set_plaintext_modulus(plaintext_modulus);
  params.set_polynomial_modulus_degree(polynomial_modulus_degree);

  compile("cryptonets.hpp", &params);

  return 0;
}
