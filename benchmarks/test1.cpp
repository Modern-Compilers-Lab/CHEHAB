#include "fhecompiler/fhecompiler.hpp"
#include <bits/stdc++.h>

using namespace std;

using namespace fhecompiler;

int main()
{
  try
  {

    fhecompiler::init("test1", fhecompiler::Scheme::bfv, fhecompiler::Backend::SEAL);

    fhecompiler::Ciphertext A("A", fhecompiler::VarType::input);
    fhecompiler::Ciphertext B("B", fhecompiler::VarType::input);
    fhecompiler::Ciphertext C("C", fhecompiler::VarType::input);
    fhecompiler::Ciphertext D("D", fhecompiler::VarType::input);

    fhecompiler::Ciphertext output1("output1", VarType::output);

    /*
    output1 = B << 3;

    for (int32_t i = 0; i < 32; i++)
      output1 += (A << (i + 1));
    */

    fhecompiler::Plaintext pt1(std::vector<int64_t>({1, 2, 4, 5, 6}));

    fhecompiler::Plaintext pt2(std::vector<int64_t>({10, 22, 34, 45, 56}));

    fhecompiler::Plaintext pt_input1("pt_input1", fhecompiler::VarType::input);
    fhecompiler::Plaintext pt_input2("pt_input2", fhecompiler::VarType::input);

    fhecompiler::Plaintext pt_output("pt_output", fhecompiler::VarType::output);

    fhecompiler::Scalar n = 3;

    /*
      (x - (y + z)) + z => (x - y)
    */

    pt_output = pt_input1 + pt_input2; //(A - (B + C)) + C;

    output1 = (5 - A) + (pt1 * pt2);

    params_selector::EncryptionParameters params;
    size_t polynomial_modulus_degree = 4096;
    size_t plaintext_modulus = 786433;

    params.set_plaintext_modulus(plaintext_modulus);
    params.set_polynomial_modulus_degree(polynomial_modulus_degree);

    fhecompiler::compile("test1.hpp", &params);
  }
  catch (const char *message)
  {
    std::cout << message << "\n";
  }

  // initial noise_budget
  return 0;
}
