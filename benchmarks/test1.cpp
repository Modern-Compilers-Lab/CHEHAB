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
    fhecompiler::Ciphertext E("E", fhecompiler::VarType::input);

    fhecompiler::Ciphertext output1("output1", VarType::output);
    fhecompiler::Ciphertext output2("output2", VarType::output);
    fhecompiler::Ciphertext output3("output3", VarType::output);
    fhecompiler::Ciphertext output4("output4", VarType::output);
    fhecompiler::Ciphertext output5("output5", VarType::output);

    fhecompiler::Plaintext pt1_const(std::vector<int64_t>({1, 3, 4}));
    fhecompiler::Plaintext pt2_const(std::vector<int64_t>({23, 3, 4}));

    auto x = fhecompiler::Ciphertext::encrypt(pt1_const);

    auto y = fhecompiler::Ciphertext::encrypt(pt2_const);

    auto z = x + y;

    z += (x * x);

    z *= z;

    z *= pt1_const;

    output1 = z;

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
  return 0;
}
