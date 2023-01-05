#include "fhecompiler/fhecompiler.hpp"
#include "frontend_tests_const.hpp"
#include <bits/stdc++.h>

inline void swap(fhecompiler::Ciphertext &ct1, fhecompiler::Ciphertext &ct2)
{
  fhecompiler::Ciphertext tmp = ct1;
  ct1 = ct2; //
  ct2 = tmp;
}

int test20_frontend()
{
  try
  {
    fhecompiler::init("test20", fhecompiler::Scheme::bfv, fhecompiler::Backend::SEAL);

    fhecompiler::Ciphertext ct1("ct1", fhecompiler::VarType::input);

    fhecompiler::Ciphertext ct2("ct2", fhecompiler::VarType::input);

    fhecompiler::Plaintext pt1("pt1", fhecompiler::VarType::input);

    fhecompiler::Ciphertext ct1_output("ct1_output", fhecompiler::VarType::output);

    fhecompiler::Ciphertext ct2_output("ct2_output", fhecompiler::VarType::output);

    fhecompiler::Ciphertext ct3_output("ct3_output", fhecompiler::VarType::output);

    fhecompiler::Ciphertext ct4_output("ct4_output", fhecompiler::VarType::output);

    fhecompiler::Ciphertext ct5_output("ct5_output", fhecompiler::VarType::output);

    fhecompiler::Ciphertext ct6_output("ct6_output", fhecompiler::VarType::output);

    ct1_output = ct1;

    ct2_output = ct1_output;

    ct3_output = ct2_output;

    ct4_output = ct2_output;

    ct5_output = ct4_output;

    ct6_output = ct5_output;

    ct6_output = ct2;

    fhecompiler::Ciphertext x("x");

    x = ct6_output + ct2_output;

    fhecompiler::Ciphertext ct7_output("ct7_output", fhecompiler::VarType::output);

    ct7_output = ct6_output + x;

    // swap(ct1_output, ct2_output);

    size_t polynomial_modulus_degree = 4096;
    size_t plaintext_modulus = 786433;

    params_selector::EncryptionParameters params;
    params.set_plaintext_modulus(plaintext_modulus);
    params.set_polynomial_modulus_degree(polynomial_modulus_degree);

    fhecompiler::compile(out_dir + "/" + "test20.hpp", &params);
  }
  catch (const char *emessage)
  {
    std::cout << emessage << '\n';
  }

  return 0;
}
