#include "fhecompiler/fhecompiler.hpp"
#include "frontend_tests_const.hpp"
#include <bits/stdc++.h>

int test4_frontend()
{

  try
  {
    fhecompiler::init("test4", fhecompiler::Scheme::bfv, fhecompiler::Backend::SEAL);

    fhecompiler::Ciphertext ct1("ct1", fhecompiler::VarType::input);

    fhecompiler::Ciphertext ct_output("ct_output", fhecompiler::VarType::output);

    fhecompiler::Ciphertext ct2;

    fhecompiler::Plaintext pt1;

    fhecompiler::Plaintext pt_input("pt_input", fhecompiler::VarType::input);

    pt1 = pt_input;

    ct2 = ct1 + pt1 + exponentiate(ct1, 3);

    ct_output = ct2;

    size_t polynomial_modulus_degree = 4096;
    size_t plaintext_modulus = 786433;

    params_selector::EncryptionParameters params;
    params.set_plaintext_modulus(plaintext_modulus);
    params.set_polynomial_modulus_degree(polynomial_modulus_degree);

    fhecompiler::compile(out_dir + "/" + "test4.hpp", &params);
  }
  catch (const char *emessage)
  {
    std::cout << emessage << '\n';
  }

  return 0;
}
