#include "fhecompiler.hpp"
#include <bits/stdc++.h>

using namespace std;

using namespace fhecompiler;

Ciphertext sum(const Ciphertext &a, const Ciphertext &b)
{
  return a + b;
}

void print_string(std::optional<std::string> string_opt)
{
  if (string_opt != nullopt)
    std::cout << *string_opt << "\n";
}

int main()
{

  /*
    Encrypt, Decrypt, Evaluate(add, mul, sub, rotate), generate keys
  */

  try
  {

    /*
      Scalars as inputs (yes or no ?)
    */

    fhecompiler::init("test1", 2 << 14, fhecompiler::Scheme::bfv);

    fhecompiler::Ciphertext output1("output1", VarType::output);

    // fhecompiler::params.set_coef_modulus({50, 50, 50});
    // a good value for t, 786433
    fhecompiler::params.set_plaintext_modulus(786433);
    // fhecompiler::params.set_plaintext_modulus_bit_length(20);
    fhecompiler::params.set_polynomial_modulus_degree(1 << 15);

    fhecompiler::Plaintext pt1(std::vector<int64_t>({6}));

    // fhecompiler::Plaintext pt2(std::vector<int64_t>({11}));

    fhecompiler::Ciphertext x = fhecompiler::Ciphertext::encrypt(pt1);
    // fhecompiler::Ciphertext y = fhecompiler::Ciphertext::encrypt(pt2);

    // fhecompiler::Ciphertext z("z");

    // 4x^4 + 8x^3 + 8x^2 + 8x + 4

    // (x-y)3 = x3 - y3 - 3x2y + 3xy2

    // 4(x^2+1)(x+1)^2

    output1 = 4 * exponentiate(x, 4) + 8 * exponentiate(x, 3) + 8 * square(x) + 8 * x + 4;
    fhecompiler::compile("test1.hpp");
  }
  catch (const char *message)
  {
    std::cout << message << "\n";
  }

  // initial noise_budget
  return 0;
}
