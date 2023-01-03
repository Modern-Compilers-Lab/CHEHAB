#include "fhecompiler/fhecompiler.hpp"
#include "frontend_tests_const.hpp"
#include <bits/stdc++.h>

int test14_frontend()
{

  try
  {
    fhecompiler::init("test14", fhecompiler::Scheme::bfv, fhecompiler::Backend::SEAL);

    fhecompiler::Ciphertext ct1("ct1", fhecompiler::VarType::input);

    fhecompiler::Ciphertext ct2("ct2", fhecompiler::VarType::input);

    fhecompiler::Ciphertext ct1_output("ct1_output", fhecompiler::VarType::output);

    fhecompiler::Ciphertext ct2_output("ct2_output", fhecompiler::VarType::output);

    fhecompiler::Ciphertext x("x");
    fhecompiler::Ciphertext y("y");

    x = ct1 * ct1 + (ct1 << 1);

    ct1_output = x * x + (ct2 << 2);

    ct2_output = ct1_output * ct1_output;

    y = x * (ct2_output << 5);

    ct2_output <<= 4;

    fhecompiler::Ciphertext ct3_output("ct3_output", fhecompiler::VarType::output);

    ct3_output = ct2_output + 213;

    ct3_output <<= 7;

    // ct3_output.square();

    ct3_output += (ct1_output + ct2_output + y);

    size_t polynomial_modulus_degree = 4096;
    size_t plaintext_modulus = 786433;

    params_selector::EncryptionParameters params;
    params.set_plaintext_modulus(plaintext_modulus);
    params.set_polynomial_modulus_degree(polynomial_modulus_degree);

    fhecompiler::compile(out_dir + "/" + "test14.hpp", &params);
  }
  catch (const char *emessage)
  {
    std::cout << emessage << '\n';
  }

  return 0;
}
