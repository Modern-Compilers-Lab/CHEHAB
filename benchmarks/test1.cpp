#include "fhecompiler.hpp"
#include <bits/stdc++.h>

using namespace std;

using namespace fhecompiler;

Ciphertext sum(const Ciphertext &a, const Ciphertext &b)
{
  return a + b;
}

void print_string(std::optional<std::string> string_opt)
{
  if (string_opt != nullopt)
    std::cout << *string_opt << "\n";
}

int main()
{

  /*
    Encrypt, Decrypt, Evaluate(add, mul, sub, rotate), generate keys
  */

  try
  {
    fhecompiler::init("test1", 1024, fhecompiler::bfv);

    // scheme better to be introduced in init
    // fhecompiler::Plaintext pt1(std::vector<int64_t>{1, 3, 4, 5});

    fhecompiler::Plaintext pt1("pt1", VarType::input);

    fhecompiler::Plaintext pt2("pt2_input", VarType::input);

    fhecompiler::Plaintext pt3("pt3", VarType::input); // temp is default type

    // fhecompiler::Ciphertext ct1 = fhecompiler::Ciphertext::encrypt(pt2);

    fhecompiler::Ciphertext ct_input1("ct_input1", VarType::input);

    fhecompiler::Ciphertext ct_input2("ct_input2", VarType::input);

    fhecompiler::Ciphertext ct_output("ct_output", VarType::output);
    fhecompiler::Ciphertext ct2 = ct_input2 + pt1 + ct_input2 * pt2 * pt3 + ct_input1;
    ct2 += ct2;
    fhecompiler::Ciphertext ct_for_debug("ct_to_debug", VarType::output);
    ct_for_debug = ct2;

    fhecompiler::compile();
  }
  catch (const char *message)
  {
    std::cout << message << "\n";
  }

  return 0;
}
