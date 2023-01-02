#include "fhecompiler/fhecompiler.hpp"
#include "frontend_tests_const.hpp"
#include <bits/stdc++.h>

inline fhecompiler::Ciphertext fibo()
{
  /*
    This function computes fibonacci, it was created only for testing
  */

  size_t n = 20;

  fhecompiler::Plaintext pt1(std::vector<int64_t>({0}));
  fhecompiler::Plaintext pt2(std::vector<int64_t>({1}));

  fhecompiler::Ciphertext x = fhecompiler::Ciphertext::encrypt(pt1);
  fhecompiler::Ciphertext y = fhecompiler::Ciphertext::encrypt(pt2);

  fhecompiler::Ciphertext ct1 = y;
  fhecompiler::Ciphertext ct2 = x + y;

  /*
  ct1 = y;
  ct2 = x + y;
  */

  fhecompiler::Ciphertext z("z");

  n -= 2;
  while (n--)
  {
    fhecompiler::Ciphertext ct3 = ct1 + ct2;
    ct1 = ct2;
    ct2 = ct3;
  }

  return ct2;
}

int test13_frontend()
{

  try
  {
    fhecompiler::init("test13", fhecompiler::Scheme::bfv, fhecompiler::Backend::SEAL);

    fhecompiler::Ciphertext ct1("ct1", fhecompiler::VarType::temp);

    fhecompiler::Ciphertext ct2("ct2", fhecompiler::VarType::temp);

    fhecompiler::Ciphertext ct1_output("ct_output", fhecompiler::VarType::output);

    ct1_output = fibo();

    size_t polynomial_modulus_degree = 4096;
    size_t plaintext_modulus = 786433;

    params_selector::EncryptionParameters params;
    params.set_plaintext_modulus(plaintext_modulus);
    params.set_polynomial_modulus_degree(polynomial_modulus_degree);

    fhecompiler::compile(out_dir + "/" + "test13.hpp", &params);
  }
  catch (const char *emessage)
  {
    std::cout << emessage << '\n';
  }

  return 0;
}
