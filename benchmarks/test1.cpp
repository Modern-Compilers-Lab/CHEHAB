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

    fhecompiler::Plaintext E("E", fhecompiler::VarType::input);

    fhecompiler::Ciphertext output1("output1", VarType::output);

    fhecompiler::Plaintext plain1 = std::vector<int64_t>{1, 2, 4, 5};
    fhecompiler::Plaintext plain2 = std::vector<int64_t>{1, 2, 4};

    output1 = (A + plain1) + plain2;

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
