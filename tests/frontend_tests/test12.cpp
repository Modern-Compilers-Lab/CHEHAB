#include "fhecompiler/fhecompiler.hpp"
#include "frontend_tests_const.hpp"
#include <bits/stdc++.h>

inline fhecompiler::Ciphertext random_computation(
  const fhecompiler::Ciphertext &ct1, const fhecompiler::Ciphertext &ct2)
{
  fhecompiler::Ciphertext ct_tmp("ct_tmp", fhecompiler::VarType::temp);

  ct_tmp = ct1 + ((ct2 << 1) >> 1);

  ct_tmp = fhecompiler::square(ct_tmp);

  fhecompiler::Ciphertext ct_tmp_output("ct_tmp_output", fhecompiler::VarType::output);

  ct_tmp_output = ct_tmp;

  return ct_tmp_output;
}

int test12_frontend()
{

  try
  {
    fhecompiler::init("test12", fhecompiler::Scheme::bfv, fhecompiler::Backend::SEAL);

    fhecompiler::Ciphertext ct1("ct1", fhecompiler::VarType::input);

    fhecompiler::Ciphertext ct2("ct2", fhecompiler::VarType::input);

    fhecompiler::Ciphertext ct2_otuput("ct2_output", fhecompiler::VarType::output);
    fhecompiler::Ciphertext ct1_output("ct1_output", fhecompiler::VarType::output);

    ct1_output = random_computation(ct1, ct2);

    ct2_otuput = ct1_output * ct1_output;

    size_t polynomial_modulus_degree = 4096;
    size_t plaintext_modulus = 786433;

    params_selector::EncryptionParameters params;
    params.set_plaintext_modulus(plaintext_modulus);
    params.set_polynomial_modulus_degree(polynomial_modulus_degree);

    fhecompiler::compile(out_dir + "/" + "test12.hpp", &params);
  }
  catch (const char *emessage)
  {
    std::cout << emessage << '\n';
  }

  return 0;
}
