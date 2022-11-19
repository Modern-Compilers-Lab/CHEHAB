#include "fhecompiler.hpp"
#include <bits/stdc++.h>

using namespace std;

using namespace fhecompiler;

Ciphertext sum(const Ciphertext &a, const Ciphertext &b)
{
  return a + b;
}

void swap(fhecompiler::Ciphertext &a, fhecompiler::Ciphertext &b)
{
  fhecompiler::Ciphertext t = a;
  // t = a assign
  // a =
  a = b;
  b = t;
}

Ciphertext sum_all_slots(fhecompiler::Ciphertext &x)
{
  // number of slots needs to be know by the user
  // we assume here that we have 5 slots in x
  fhecompiler::Ciphertext result("sum_result", VarType::temp);
  result = x;
  x <<= 1;
  result += x;
  x <<= 1;
  result += x;
  x <<= 1;
  result += x;
  x <<= 1;
  result += x;
  // result of sum will be on the first slot
  return result;
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

    fhecompiler::init("test1", 1 << 10, fhecompiler::Scheme::bfv);

    fhecompiler::Ciphertext output1("output1", VarType::output);

    // fhecompiler::params.set_coef_modulus({50, 50, 50});
    // a good value for t, 786433
    fhecompiler::params.set_plaintext_modulus(786433);
    // fhecompiler::params.set_plaintext_modulus_bit_length(20);
    fhecompiler::params.set_polynomial_modulus_degree(1 << 15);

    fhecompiler::Plaintext pt1(std::vector<int64_t>({0}));

    fhecompiler::Plaintext pt2(std::vector<int64_t>({1}));

    fhecompiler::Plaintext pt3(std::vector<int64_t>({1, 2, 3, 4, 5}));

    fhecompiler::Plaintext pt4(std::vector<int64_t>({3, 4, 5, 6, 6}));

    fhecompiler::Ciphertext x = fhecompiler::Ciphertext::encrypt(pt1);
    fhecompiler::Ciphertext y = fhecompiler::Ciphertext::encrypt(pt2);

    // fhecompiler::Ciphertext z("z");
    //  fhecompiler::Ciphertext z("z");

    // 4x^4 + 8x^3 + 8x^2 + 8x + 4

    // (x-y)3 = x3 - y3 - 3x2y + 3xy2

    // 4(x^2+1)(x+1)^2
    // let's compute fibonacci

    size_t n = 20;

    fhecompiler::Ciphertext ct1 = y;
    fhecompiler::Ciphertext ct2 = x + y;

    fhecompiler::Ciphertext z("z");

    // fhecompiler::Ciphertext output2("output2")

    n -= 2;
    while (n--)
    {
      Ciphertext ct3 = ct1 + ct2;
      ct1 = ct2;
      ct2 = ct3;
      if (n == 10)
        z = ct2;
    }

    fhecompiler::Ciphertext k = z;

    std::cout << z.get_label() << "\n";

    fhecompiler::Ciphertext ct3 = Ciphertext::encrypt(pt3);

    fhecompiler::Ciphertext ct4 = Ciphertext::encrypt(pt4);

    // fhecompiler::Ciphertext product_result("product_result", VarType::output);
    fhecompiler::Ciphertext product_result = (ct3 * ct4);
    // sum_all_slots(product_result);
    output1 = sum_all_slots(product_result); //(z + ct2 - ct1 + ct1 - ct2) * pt2; // ct2 + z + k;

    fhecompiler::Ciphertext output2("output2", VarType::output);
    output2 = z;
    // swap(output2, z);

    output2 *= output2;

    fhecompiler::compile("test1.hpp");
  }
  catch (const char *message)
  {
    std::cout << message << "\n";
  }

  // initial noise_budget
  return 0;
}
