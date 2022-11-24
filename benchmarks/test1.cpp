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

std::vector<int64_t> flatten_image(const std::vector<vector<int64_t>> img)
{
  // this is the encoding procedure
  std::vector<int64_t> encoded_image;
  for (auto &row : img)
  {
    for (auto &e : row)
      encoded_image.push_back(e);
  }
  return encoded_image;
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
  // result of sum will be in the first slot
  return result;
}

Ciphertext dotProduct(fhecompiler::Ciphertext &ct1, fhecompiler::Ciphertext &ct2)
{
  fhecompiler::Ciphertext simd_product = ct1 * ct2;
  return sum_all_slots(simd_product);
}

/*
1 Ctxt foo_batched ( Ctxt img ) {
2 Ctxt r0 = img * -8;
3 Ctxt r1 = img << -n -1;
4 Ctxt r2 = img << -n;
5 Ctxt r3 = img << -n +1;
6 Ctxt r4 = img << -1;
7 Ctxt r5 = img << 1;
8 Ctxt r6 = img << n -1;
9 Ctxt r7 = img << n;
10 Ctxt r8 = img << n +1;
11 return 2* img -( r0+r1+r2+r3+r4+r5+r6+r7+r8) ;
12 }
*/

Ciphertext simple_sharpening_filter(fhecompiler::Ciphertext &img, int n)
{
  /*
    filter : {
      {1 ,1 ,1},
      {1 , -8 ,1},
      {1 ,1 ,1}
      }
  */
  fhecompiler::Ciphertext r0 = img * -8;
  fhecompiler::Ciphertext r1 = img << -n - 1;
  fhecompiler::Ciphertext r2 = img << -n;
  fhecompiler::Ciphertext r3 = img << -n + 1;
  fhecompiler::Ciphertext r4 = img << -1;
  fhecompiler::Ciphertext r5 = img << 1;
  fhecompiler::Ciphertext r6 = img << n - 1;
  fhecompiler::Ciphertext r7 = img << n;
  fhecompiler::Ciphertext r8 = img << n + 1;
  return 2 * img - (r0 + r1 + r2 + r3 + r4 + r5 + r6 + r7 + r8);
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

    fhecompiler::init("test1", 0, 4096, fhecompiler::Scheme::bfv, Backend::SEAL);

    fhecompiler::Ciphertext output1("output1", VarType::output);

    fhecompiler::params.set_coef_modulus({50, 50, 50});
    // a good value for t, 786433
    fhecompiler::params.set_plaintext_modulus(786433);
    // fhecompiler::params.set_plaintext_modulus_bit_length(20);
    fhecompiler::params.set_polynomial_modulus_degree(4096 * 2);

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

    std::cout << z.get_label() << "\n";

    fhecompiler::Ciphertext ct3 = Ciphertext::encrypt(pt3);

    fhecompiler::Ciphertext ct4 = Ciphertext::encrypt(pt4);

    // fhecompiler::Ciphertext product_result("product_result", VarType::output);
    fhecompiler::Ciphertext product_result("product_result");
    product_result = (ct3 * ct4);
    // sum_all_slots(product_result);
    // output1 = sum_all_slots(product_result); //(z + ct2 - ct1 + ct1 - ct2) * pt2; // ct2 + z + k;
    /*
    output1 = dotProduct(ct3, ct4);

    product_result += z;

    fhecompiler::Ciphertext output2("output2", VarType::output);
    output2 = z + product_result;
    // swap(output2, z);

    output2 *= output2;
    */
    std::vector<vector<int64_t>> img_clear = {{1, 2, 3, 4}, {5, 6, 7, 8}, {1, 2, 4, 7}, {12, 1, 2, 2}};

    // flatten image
    std::vector<int64_t> img_encoded = flatten_image(img_clear);

    fhecompiler::Plaintext img_plain(img_encoded);

    fhecompiler::Ciphertext img_cipher = Ciphertext::encrypt(img_plain);

    fhecompiler::Ciphertext output3("output3", VarType::output);
    output3 = simple_sharpening_filter(img_cipher, 4);

    /*
    output1 = (ct1 + ct2 - ct1 + ct2 + ct3 - ct4 - (-ct1) + ct1 - ct2 - ct2 - ct2) * 2 +
              (ct1 + ct2 - ct1 + ct2 + ct3 - ct4 - (-ct1) + ct1 - ct2 - ct2 - ct2) * 2 -
              (ct1 + ct2 - ct1 + ct2 + ct3 - ct4 - (-ct1) + ct1 - ct2 - ct2 - ct2) * 2;
    */
    fhecompiler::compile("test1.hpp");
  }
  catch (const char *message)
  {
    std::cout << message << "\n";
  }

  // initial noise_budget
  return 0;
}
