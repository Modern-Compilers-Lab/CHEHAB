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

    fhecompiler::Ciphertext output1("output1", VarType::output);

    output1 = (A << 3) * (B << 5) + ((A << 3) + (B << 3));

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
