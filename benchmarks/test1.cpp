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
    fhecompiler::init("test1", 1024, fhecompiler::Scheme::bfv);

    // fhecompiler::Plaintext pt11(std::vector<int64_t>{1, 3, 4, 5});

    fhecompiler::Ciphertext ct1("ct1", VarType::input);

    fhecompiler::Plaintext pt1("pt1", VarType::input);

    fhecompiler::Ciphertext output1("output1", VarType::output);

    fhecompiler::Ciphertext output2("output2", VarType::output);

    output2 = fhecompiler::Ciphertext::encrypt(pt1);

    output1 = (ct1 + pt1) * 2 + ct1 + ct1 + pt1;

    output1 += 1312;

    fhecompiler::Ciphertext ct3_output = std::move(output1);

    fhecompiler::Ciphertext output3("output3", VarType::output);

    int rotation_steps = 12;

    output3 = ct1 >> rotation_steps;

    output3 <<= (int)5;

    /*
      {1, 3, 4}

      ct1 + pt1 -> {2, 6, 8}
      (ct1 + pt1) * 2 -> {4, 0, 0}
      ct1 + ct1 -> {2, 6, 8}
      ct1 + ct1 + pt1 -> {3, 9, 12}
      (ct1 + pt1) * 2 + ct1 + ct1 + pt1 -> {7, 9, 12} final result
      ((ct1 + pt1) * 2 + ct1 + ct1 + pt1) + 1312 -> {1319, 9, 12}
    */

    fhecompiler::compile("test1.hpp");
  }
  catch (const char *message)
  {
    std::cout << message << "\n";
  }

  return 0;
}
