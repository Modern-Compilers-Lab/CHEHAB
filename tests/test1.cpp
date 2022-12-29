#include "fhecompiler/fhecompiler.hpp"
#include <bits/stdc++.h>

using namespace std;

int main()
{
  try
  {
    fhecompiler::init("test1", fhecompiler::Scheme::bfv, fhecompiler::Backend::SEAL);

    // in this testcase, data is provided in compile time
    fhecompiler::Ciphertext x = fhecompiler::Ciphertext::encrypt(std::vector<int64_t>({1, 2, 3, 4, 5, 6}));
    fhecompiler::Ciphertext y = fhecompiler::Ciphertext::encrypt(std::vector<int64_t>({2, 3, 4, 5, 6, 7}));

    fhecompiler::Ciphertext sum_output("sum_output", fhecompiler::VarType::output);
    sum_output = x + y;

    size_t polynomial_modulus_degree = 4096;
    size_t plaintext_modulus = 786433;

    params_selector::EncryptionParameters params;
    params.set_plaintext_modulus(plaintext_modulus);
    params.set_polynomial_modulus_degree(polynomial_modulus_degree);
    // please note that coef modulus will be selected by default by SEAL

    fhecompiler::compile("test1.hpp", &params);
  }
  catch (char *emessage)
  {
    std::cerr << emessage << "\n";
  }
}
